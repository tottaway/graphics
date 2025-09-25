#include "lightmaze/map/map_entity.hh"
#include "components/center.hh"
#include "components/collider.hh"
#include "components/draw_rectangle.hh"
#include "components/jump_reset.hh"
#include "geometry/rectangle_utils.hh"
#include "lightmaze/components/lightmaze_collider.hh"
#include "lightmaze/components/lightmaze_light_volume.hh"
#include "lightmaze/map/map_mode_manager.hh"
#include "model/game_state.hh"
#include "view/screen.hh"
#include <SFML/Window/Keyboard.hpp>

namespace lightmaze {

MapEntity::MapEntity(model::GameState &game_state)
    : model::Entity(game_state) {}

Result<void, std::string> MapEntity::init(const Params &params) {
  entity_params_ = params.entity_params;

  return std::visit(
      [this](const auto &specific_params) -> Result<void, std::string> {
        using T = std::decay_t<decltype(specific_params)>;

        if constexpr (std::is_same_v<T, PlatformParams>) {
          return init_as_platform(specific_params);
        } else if constexpr (std::is_same_v<T, SerializedMapEntityParams>) {
          return init_from_yaml(specific_params.yaml_node);
        } else {
          return Err(std::string("Unknown entity type in MapEntity::init"));
        }
      },
      entity_params_);
}

Result<void, std::string>
MapEntity::init_as_platform(const PlatformParams &platform_params) {
  // Store the size and calculate position
  // Calculate the actual center position from the top center position
  // Move down by half the height to get the center
  position_ = platform_params.top_center_position -
              Eigen::Vector2f{0.0f, platform_params.size.y()};
  color_ = platform_params.platform_color;

  // Add white rectangle drawing component for visual representation
  add_component<component::DrawRectangle>([this]() {
    return component::DrawRectangle::RectangleInfo{
        .transform = get_transform(),
        .color = color_,
    };
  });

  add_collider_components();
  add_light_detector_component();

  return Ok();
}

void MapEntity::add_collider_components() {
  // Add solid collision component so player can stand on platform
  add_component<component::SolidAABBCollider>(
      [this]() { return get_transform(); },
      [this](const Eigen::Vector2f &translation) {
        // For platforms, we don't want them to move when hit
        // so we ignore the translation callback
      });

  // Add jump reset component on the top surface for ground detection
  add_component<component::JumpReset>([this]() {
    auto platform_params = std::get<PlatformParams>(entity_params_);
    Eigen::Affine2f transform = get_transform();

    // Position at the top center of the platform
    transform.translate(Eigen::Vector2f{0.0f, 0.98});

    // Scale to platform width but very thin height for top surface
    // detection
    auto scale_factor = 0.02 / platform_params.size.y();
    transform.scale(Eigen::Vector2f{1., scale_factor});

    return transform;
  });
}

void MapEntity::add_light_detector_component() {
  // Add LightMazeCollider for color-based collision mechanics
  add_component<component::LightMazeCollider>(
      [this]() {
        Eigen::Affine2f transform = this->get_transform();
        transform.scale(1.1);
        return transform;
      },      // transform function
      color_, // platform color from parameters
      [this](const model::EntityID entity_id) {
        const auto maybe_light_volume_entity =
            game_state_.try_get_entity_pointer_by_id(entity_id);
        if (!maybe_light_volume_entity.has_value()) {
          return;
        }
        const auto components = maybe_light_volume_entity.value()
                                    ->get_components<component::Collider>();

        for (const auto component : components) {
          if (component->get_collider_type_name() ==
                  component::LightMazeLightVolume::collider_type_name &&
              dynamic_cast<component::LightMazeLightVolume *>(component)
                      ->get_light_color() == color_) {
            this->is_illuminated_ = true;
          }
        }
      });
}

Result<void, std::string> MapEntity::update(const int64_t delta_time_ns) {
  // Update all components
  if (is_illuminated_ != was_illuminated_last_frame_) {
    if (is_illuminated_) {
      remove_components<component::Collider>();
      // the light detector is also a collider so we need to add it back
      add_light_detector_component();
    } else {
      add_collider_components();
    }
    was_illuminated_last_frame_ = is_illuminated_;
  }
  is_illuminated_ = false;

  for (const auto &component : components_) {
    TRY_VOID(component->update(delta_time_ns));
  }

  return Ok();
}

Eigen::Affine2f MapEntity::get_transform() const {
  auto platform_params = std::get<PlatformParams>(entity_params_);

  Eigen::Affine2f transform = Eigen::Affine2f::Identity();
  transform.translate(position_);
  transform.scale(platform_params.size);

  return transform;
}

Eigen::Vector2f MapEntity::get_top_center_position() const {
  auto platform_params = std::get<PlatformParams>(entity_params_);
  return platform_params.top_center_position;
}

Eigen::Vector2f MapEntity::get_size() const {
  auto transform = get_transform();
  auto [bottom_left, top_right] =
      geometry::get_bottom_left_and_top_right_from_transform(transform);
  return top_right - bottom_left;
}

YAML::Node MapEntity::serialize() const {
  YAML::Node node;

  node["color"]["r"] = color_.r;
  node["color"]["g"] = color_.g;
  node["color"]["b"] = color_.b;
  return std::visit(
      [&node](const auto &specific_params) -> YAML::Node {
        using T = std::decay_t<decltype(specific_params)>;

        if constexpr (std::is_same_v<T, PlatformParams>) {
          node["type"] = "platform";
          node["top_center_position"]["x"] =
              specific_params.top_center_position.x();
          node["top_center_position"]["y"] =
              specific_params.top_center_position.y();
          node["size"]["x"] = specific_params.size.x();
          node["size"]["y"] = specific_params.size.y();
        }

        return node;
      },
      entity_params_);
}

Result<void, std::string>
MapEntity::init_from_yaml(const YAML::Node &yaml_node) {
  try {
    if (!yaml_node["type"]) {
      return Err(std::string("YAML node missing 'type' field"));
    }

    std::string type = yaml_node["type"].as<std::string>();

    if (type == "platform") {
      PlatformParams platform_params{};

      if (yaml_node["top_center_position"]) {
        platform_params.top_center_position.x() =
            yaml_node["top_center_position"]["x"].as<float>();
        platform_params.top_center_position.y() =
            yaml_node["top_center_position"]["y"].as<float>();
      }

      if (yaml_node["size"]) {
        platform_params.size.x() = yaml_node["size"]["x"].as<float>();
        platform_params.size.y() = yaml_node["size"]["y"].as<float>();
      }

      if (yaml_node["color"]) {
        platform_params.platform_color.r = yaml_node["color"]["r"].as<int>();
        platform_params.platform_color.g = yaml_node["color"]["g"].as<int>();
        platform_params.platform_color.b = yaml_node["color"]["b"].as<int>();
      }

      entity_params_ = platform_params;
      return init_as_platform(platform_params);
    }

    return Err(std::string("Unknown entity type in YAML: ") + type);

  } catch (const std::exception &e) {
    return Err(std::string("Failed to parse YAML: ") + e.what());
  }
}

Result<bool, std::string>
MapEntity::on_mouse_down(const view::MouseDownEvent &event) {
  // Only handle right-click events
  if (event.button != view::MouseButton::Right) {
    return Ok(true); // Event not handled, continue processing
  }

  if (!geometry::rectangle_contains_point(get_transform(), event.position)) {
    return Ok(true);
  }

  // Check if we're in editor mode by finding the MapModeManager
  auto mode_manager_result =
      game_state_.get_entity_pointer_by_type<MapModeManager>();
  if (mode_manager_result.isErr() ||
      !mode_manager_result.unwrap()->is_editor_mode()) {
    return Ok(true); // Not in editor mode, ignore event
  }

  // Check for double-click deletion
  auto now = std::chrono::steady_clock::now();
  auto time_since_last_click_ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(
          now - last_right_click_time_)
          .count();

  if (time_since_last_click_ns <= double_click_threshold_ns_) {
    // Double-click detected - delete this platform
    remove_entity(get_entity_id());
    return Ok(false); // Event handled, stop processing
  }

  // Single click - start dragging
  last_right_click_time_ = now;
  is_being_dragged_ = true;
  drag_offset_ = event.position; // Store starting mouse position

  return Ok(false); // Event handled, stop processing
}

Result<bool, std::string>
MapEntity::on_mouse_up(const view::MouseUpEvent &event) {
  // Only handle right-click release during dragging
  if (event.button != view::MouseButton::Right || !is_being_dragged_) {
    return Ok(true); // Event not handled, continue processing
  }

  // End dragging
  is_being_dragged_ = false;

  return Ok(false); // Event handled, stop processing
}

Result<bool, std::string>
MapEntity::on_mouse_moved(const view::MouseMovedEvent &event) {
  // Only handle movement if we're being dragged
  if (!is_being_dragged_) {
    return Ok(true); // Event not handled, continue processing
  }

  // Calculate mouse movement delta
  Eigen::Vector2f mouse_delta = event.position - drag_offset_;

  // Update platform position by the mouse delta
  if (!std::holds_alternative<PlatformParams>(entity_params_)) {
    return Err(
        std::string("MapEntity not configured as platform for mouse movement"));
  }
  auto platform_params = std::get<PlatformParams>(entity_params_);
  Eigen::Vector2f new_top_center_position =
      platform_params.top_center_position + mouse_delta;
  set_position(new_top_center_position);

  // Update drag offset for next movement
  drag_offset_ = event.position;

  return Ok(false); // Event handled, stop processing
}

Result<bool, std::string>
MapEntity::on_key_press(const view::KeyPressedEvent &key_press) {
  // Only handle key presses if we're in editor mode and being dragged
  auto mode_manager_result =
      game_state_.get_entity_pointer_by_type<MapModeManager>();
  if (mode_manager_result.isErr() ||
      !mode_manager_result.unwrap()->is_editor_mode() || !is_being_dragged_) {
    return Ok(true); // Event not handled, continue processing
  }

  // Handle color selection keys (1-4)
  switch (key_press.key_event.code) {
  case sf::Keyboard::Num1:
    color_ = {0, 0, 0}; // Black
    return Ok(false);   // Event handled, stop processing

  case sf::Keyboard::Num2:
    color_ = {255, 0, 0}; // Red
    return Ok(false);     // Event handled, stop processing

  case sf::Keyboard::Num3:
    color_ = {0, 0, 255}; // Blue
    return Ok(false);     // Event handled, stop processing

  case sf::Keyboard::Num4:
    color_ = {0, 255, 0}; // Green
    return Ok(false);     // Event handled, stop processing

  default:
    return Ok(true); // Event not handled, continue processing
  }
}

void MapEntity::set_position(const Eigen::Vector2f &new_top_center_position) {
  // Update the platform parameters
  if (!std::holds_alternative<PlatformParams>(entity_params_)) {
    std::cerr << "Error: set_position called on MapEntity not configured as "
                 "platform"
              << std::endl;
    return; // Cannot set position on non-platform entities
  }
  auto &platform_params = std::get<PlatformParams>(entity_params_);
  platform_params.top_center_position = new_top_center_position;

  // Recalculate the center position from the top center position
  position_ =
      new_top_center_position - Eigen::Vector2f{0.0f, platform_params.size.y()};
}

} // namespace lightmaze
