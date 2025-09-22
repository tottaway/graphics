#include "lightmaze/map/map_entity.hh"
#include "components/center.hh"
#include "components/collider.hh"
#include "components/draw_rectangle.hh"
#include "components/jump_reset.hh"
#include "model/game_state.hh"
#include "view/screen.hh"

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

  // Add white rectangle drawing component for visual representation
  add_component<component::DrawRectangle>([this]() {
    return component::DrawRectangle::RectangleInfo{
        .transform = get_transform(),
        .color = view::Color{255, 255, 255} // White color
    };
  });

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

  return Ok();
}

Result<void, std::string> MapEntity::update(const int64_t delta_time_ns) {
  // Update all components
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
  auto platform_params = std::get<PlatformParams>(entity_params_);
  return platform_params.size;
}

YAML::Node MapEntity::serialize() const {
  YAML::Node node;

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
      PlatformParams platform_params;

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

      entity_params_ = platform_params;
      return init_as_platform(platform_params);
    }

    return Err(std::string("Unknown entity type in YAML: ") + type);

  } catch (const std::exception &e) {
    return Err(std::string("Failed to parse YAML: ") + e.what());
  }
}

} // namespace lightmaze
