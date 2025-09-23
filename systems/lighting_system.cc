#include "systems/lighting_system.hh"
#include "components/light_emitter.hh"
#include "geometry/rectangle_utils.hh"
#include <iostream>

namespace systems {

Result<void, std::string> LightingSystem::update(model::GameState &game_state,
                                                 const int64_t delta_time_ns) {
  // Clear previous frame's lights
  active_lights_.clear();

  // Get all entities with LightEmitter components
  const auto entities =
      game_state.get_entities_with_component<component::LightEmitter>();

  for (const auto *entity : entities) {
    if (!entity)
      continue;

    const auto light_emitters =
        entity->get_components<component::LightEmitter>();
    for (const auto *light_emitter : light_emitters) {
      if (!light_emitter)
        continue;

      // Get current light info and store for rendering
      const auto light_info = light_emitter->get_light_info();

      // Only include lights with positive intensity
      if (light_info.intensity > 0.0f) {
        active_lights_.push_back(light_info);
      }
    }
  }

  return Ok();
}

Result<void, std::string> LightingSystem::draw(view::Screen &screen) {
  // Simple approach: draw a dark overlay first, then bright lights on top
  const float viewport_size = 2.5f;
  const Eigen::Vector2f viewport_bottom_left{-viewport_size, -viewport_size};
  const Eigen::Vector2f viewport_top_right{viewport_size, viewport_size};

  // Draw dark overlay using existing draw_rectangle (which we know works)
  const view::Color dark_overlay{50, 50, 50}; // Dark gray overlay
  screen.draw_rectangle(viewport_bottom_left, viewport_top_right, dark_overlay, 2.0f);

  // Render all collected lights as bright overlays using existing draw_rectangle
  for (const auto &light_info : active_lights_) {
    // Check geometry type and render appropriately
    if (!light_info.geometry) {
      return Err(std::string("Light has null geometry"));
    }

    const auto geometry_type = light_info.geometry->get_geometry_type();
    if (geometry_type == component::CircularLightGeometry::geometry_type_name) {
      TRY_VOID(render_circular_light(screen, light_info));
    } else {
      return Err(std::string("Unhandled light geometry type: ") +
                 std::string(geometry_type));
    }
  }

  return Ok();
}

Result<void, std::string> LightingSystem::render_circular_light(
    view::Screen &screen,
    const component::LightEmitter::LightInfo &light_info) const {

  // Get the circular geometry to extract radius
  const auto *circular_geometry =
      dynamic_cast<const component::CircularLightGeometry *>(
          light_info.geometry.get());

  if (!circular_geometry) {
    return Err(
        std::string("Expected CircularLightGeometry but got different type"));
  }

  const float radius_meters = circular_geometry->get_radius();
  const Eigen::Vector2f position = light_info.world_position;

  // Create a transform for the light circle
  // Position at light center, scale by radius
  const auto light_transform =
      geometry::make_square_from_center_and_size(position, radius_meters);

  // Apply intensity to color
  const float effective_intensity = light_info.intensity;
  const view::Color adjusted_color{
      static_cast<int>(light_info.color.r * effective_intensity),
      static_cast<int>(light_info.color.g * effective_intensity),
      static_cast<int>(light_info.color.b * effective_intensity)};

  // Get bounds for drawing
  const auto [bottom_left, top_right] =
      geometry::get_bottom_left_and_top_right_from_transform(light_transform);

  // Draw light using existing draw_rectangle (which we know works)
  screen.draw_rectangle(bottom_left, top_right, adjusted_color, 3.0f);

  return Ok();
}

} // namespace systems
