#include "lightmaze/map/map.hh"
#include "components/light_emitter.hh"
#include "components/zoom.hh"
#include "lightmaze/map/map_entity.hh"
#include "lightmaze/map/map_mode_manager.hh"
#include "lightmaze/player.hh"
#include "model/game_state.hh"
#include "view/screen.hh"
#include <SFML/Window/Keyboard.hpp>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <yaml-cpp/yaml.h>

namespace lightmaze {

Map::Map(model::GameState &game_state) : model::Entity(game_state) {}

Result<void, std::string> Map::init() {
  // Create the mode manager first
  auto mode_manager = TRY(add_child_entity_and_init<MapModeManager>());
  mode_manager_id_ = mode_manager->get_entity_id();

  // Try to load saved state first, fall back to default if not available
  auto load_result = load_saved_state();
  if (load_result.isErr()) {
    std::cout << "No saved state found or load failed: "
              << load_result.unwrapErr() << ". Creating default level."
              << std::endl;

    // Create default platforms (same as original hardcoded ones)
    TRY_VOID(add_platform(Eigen::Vector2f{0.0f, 0.0f}, // top center position
                          Eigen::Vector2f{1.0f, 0.2f}
                          // size: 1m wide, 0.2m thick
                          ));

    TRY_VOID(add_platform(
        Eigen::Vector2f{1.5f, 0.8f}, // top center position (right and up)
        Eigen::Vector2f{1.0f, 0.2f}  // size: 1m wide, 0.2m thick
        ));
  }

  return Ok();
}

MapModeManager* Map::get_mode_manager() const {
  auto result = game_state_.get_entity_pointer_by_id_as<MapModeManager>(mode_manager_id_);
  if (result.isOk()) {
    return result.unwrap();
  }
  return nullptr;
}

Result<model::EntityID, std::string>
Map::add_platform(const Eigen::Vector2f &top_center_position,
                  const Eigen::Vector2f &size) {

  // Create MapEntity configured as a platform
  MapEntity::PlatformParams platform_params{top_center_position, size};
  MapEntity::Params params{platform_params};

  auto map_entity = TRY(add_child_entity_and_init<MapEntity>(params));

  return Ok(map_entity->get_entity_id());
}


Result<void, std::string>
Map::save_current_state(const std::string &file_path) {
  try {
    // Create saves directory if it doesn't exist
    std::filesystem::path save_path(file_path);
    std::filesystem::create_directories(save_path.parent_path());

    // Create YAML document
    YAML::Node root;
    root["format_version"] = "1.0";

    // Serialize all child map entities
    YAML::Node entities_node;
    for (const auto &child_id : get_child_entities()) {
      auto map_entity_result =
          game_state_.get_entity_pointer_by_id_as<MapEntity>(child_id);
      if (map_entity_result.isOk()) {
        auto *map_entity = map_entity_result.unwrap();
        entities_node.push_back(map_entity->serialize());
      }
    }

    root["entities"] = entities_node;

    // Write to file
    std::ofstream file(file_path);
    if (!file.is_open()) {
      return Err(std::string("Failed to open file for writing: ") + file_path);
    }

    file << root;
    file.close();

    time_since_last_save_ns_ = 0;
    return Ok();

  } catch (const std::exception &e) {
    return Err(std::string("Failed to save map: ") + e.what());
  }
}

Result<void, std::string> Map::load_saved_state(const std::string &file_path) {
  try {
    if (!std::filesystem::exists(file_path)) {
      return Err("Save file does not exist: " + file_path);
    }

    // Load YAML file
    YAML::Node root = YAML::LoadFile(file_path);

    if (!root["entities"]) {
      return Err(std::string("YAML file missing 'entities' section"));
    }

    // Load all entities using SerializedMapEntityParams
    YAML::Node entities_node = root["entities"];
    for (const auto &entity_node : entities_node) {
      MapEntity::SerializedMapEntityParams serialized_params{entity_node};
      MapEntity::Params params{serialized_params};

      auto entity_result = add_child_entity_and_init<MapEntity>(params);
      if (entity_result.isErr()) {
        std::cout << "Warning: Failed to load entity: "
                  << entity_result.unwrapErr() << std::endl;
      }
    }

    return Ok();

  } catch (const std::exception &e) {
    return Err(std::string("Failed to load map: ") + e.what());
  }
}


Result<bool, std::string>
Map::on_mouse_down(const view::MouseDownEvent &event) {
  // Only handle left-click in editor mode
  auto* mode_manager = get_mode_manager();
  if (!mode_manager || !mode_manager->is_editor_mode() ||
      event.button != view::MouseButton::Left) {
    return Ok(true); // Event not handled, continue processing
  }

  // Start platform creation
  is_creating_platform_ = true;
  creation_start_pos_ = event.position;

  return Ok(false); // Event handled, stop processing
}

Result<bool, std::string> Map::on_mouse_up(const view::MouseUpEvent &event) {
  // Only handle left-click release in editor mode during creation
  auto* mode_manager = get_mode_manager();
  if (!mode_manager || !mode_manager->is_editor_mode() ||
      event.button != view::MouseButton::Left || !is_creating_platform_) {
    return Ok(true); // Event not handled, continue processing
  }

  // Complete platform creation
  is_creating_platform_ = false;
  TRY_VOID(create_platform(creation_start_pos_, event.position));

  return Ok(false); // Event handled, stop processing
}

Result<bool, std::string>
Map::on_mouse_moved(const view::MouseMovedEvent &event) {
  // TODO: Implement platform creation preview
  // For now, just pass through
  return Ok(true); // Event not handled, continue processing
}

Result<bool, std::string>
Map::on_mouse_scroll(const view::MouseScrollEvent &event) {
  // Get the zoom component and apply zoom factor
  // Note: We apply zoom even if not in editor mode - if we're not in editor mode,
  // the zoom will be reset to 1.0 by update_editor_components() on the next frame
  auto maybe_zoom_component = get_component<component::Zoom>();
  if (maybe_zoom_component.has_value()) {
    // Convert scroll delta to zoom factor
    // Positive delta = scroll up = zoom in, negative delta = scroll down = zoom out
    const float zoom_factor = event.delta > 0.0f ? 1.1f : 0.9f;
    maybe_zoom_component.value()->apply_zoom_factor(zoom_factor);

    return Ok(false); // Event handled, stop processing other entities
  }

  return Ok(true); // No zoom component, pass through
}

Result<void, std::string> Map::update(const int64_t delta_time_ns) {
  // Update editor-specific components based on editor mode changes
  TRY_VOID(update_editor_components());

  // Update auto-save timer
  time_since_last_save_ns_ += delta_time_ns;

  // Check for auto-save
  TRY_VOID(auto_save_if_needed());

  return Ok();
}

Result<void, std::string> Map::create_platform(const Eigen::Vector2f &start,
                                               const Eigen::Vector2f &end) {
  // Calculate platform properties from drag rectangle
  Eigen::Vector2f min_corner = start.cwiseMin(end);
  Eigen::Vector2f max_corner = start.cwiseMax(end);

  // Minimum platform size (note that a rectangle of size one actually has side
  // lengths of size two)
  Eigen::Vector2f size = (max_corner - min_corner).cwiseAbs() / 2.f;
  if (size.x() < 0.01f)
    size.x() = 0.01f; // Minimum width
  if (size.y() < 0.01f)
    size.y() = 0.01f; // Minimum height

  // Top center position (platforms are positioned by their top center)
  Eigen::Vector2f top_center = Eigen::Vector2f{
      min_corner.x() + size.x(), // Center horizontally
      max_corner.y()             // Top edge
  };

  // Create the platform
  TRY_VOID(add_platform(top_center, size));

  return Ok();
}

Result<void, std::string> Map::auto_save_if_needed() {
  if (time_since_last_save_ns_ >= auto_save_interval_ns_) {
    TRY_VOID(save_current_state());
  }
  return Ok();
}

Result<void, std::string> Map::update_editor_components() {
  auto* mode_manager = get_mode_manager();
  if (!mode_manager) {
    return Ok(); // No mode manager, nothing to do
  }

  const bool is_editor_mode = mode_manager->is_editor_mode();

  if (is_editor_mode != was_in_editor_mode_) {
    // Editor mode state changed
    was_in_editor_mode_ = is_editor_mode;

    if (is_editor_mode) {
      // Entering editor mode - add global illumination and zoom
      component::LightEmitter::GlobalLightParams global_params{
        .color = {255, 255, 255}, // White light
        .intensity = 1.0f         // Full intensity
      };
      add_component<component::LightEmitter>(global_params);
      add_component<component::Zoom>(1.0f); // Start with default zoom
    } else {
      // Exiting editor mode - remove global illumination and zoom
      remove_components<component::LightEmitter>();
      remove_components<component::Zoom>();
    }
  } else if (!is_editor_mode) {
    // Not in editor mode - ensure zoom is at default level
    // This handles the case where user scrolled while not in editor mode
    auto maybe_zoom_component = get_component<component::Zoom>();
    if (maybe_zoom_component.has_value()) {
      maybe_zoom_component.value()->reset_zoom();
    }
  }

  return Ok();
}

Eigen::Affine2f Map::get_transform() const {
  // Return a large transform covering the entire play area (centered at origin)
  // This allows the Map entity to receive mouse events anywhere on screen
  Eigen::Affine2f transform = Eigen::Affine2f::Identity();
  transform.translate(Eigen::Vector2f{0.0f, 0.0f}); // Center at origin
  transform.scale(Eigen::Vector2f{20.0f, 20.0f});   // Large 20x20 meter area
  return transform;
}

} // namespace lightmaze
