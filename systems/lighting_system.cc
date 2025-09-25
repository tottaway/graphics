#include "systems/lighting_system.hh"
#include "components/light_emitter.hh"
#include <algorithm>
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
  // Lazy-load shader on first draw call
  TRY_VOID(ensure_shader_loaded());

  // Skip rendering if shader failed to load
  if (!lighting_shader_ || !lighting_shader_->is_valid()) {
    return Ok();
  }

  // Set up shader uniforms with current lights and screen info
  TRY_VOID(set_lighting_uniforms(screen));

  // Render fullscreen lighting shader on top of game entities with
  // multiplicative blending
  screen.draw_fullscreen_lighting_shader(*lighting_shader_, 1.0f);

  return Ok();
}

Result<void, std::string> LightingSystem::ensure_shader_loaded() {
  // Only attempt loading once
  if (shader_load_attempted_) {
    return Ok();
  }

  shader_load_attempted_ = true;

  // Load shader from files
  auto shader_result = view::Shader::from_files(
      std::string(vertex_shader_path_), std::string(fragment_shader_path_));

  if (shader_result.isOk()) {
    lighting_shader_ = std::move(shader_result).unwrap();
  } else {
    std::cerr << "Failed to load lighting shader: " << shader_result.unwrapErr()
              << std::endl;
    // System will be non-functional but won't crash
  }

  return Ok();
}

Result<void, std::string>
LightingSystem::set_lighting_uniforms(const view::Screen &screen) const {
  if (!lighting_shader_) {
    return Err(std::string("Lighting shader not loaded"));
  }

  // Set viewport uniforms from screen
  lighting_shader_->set_uniform("viewport_center",
                                screen.get_viewport_center());
  lighting_shader_->set_uniform("viewport_size", screen.get_actual_viewport_size());

  // Set light count
  const int32_t light_count =
      std::min(static_cast<int32_t>(active_lights_.size()), 32);
  lighting_shader_->set_uniform("light_count", light_count);

  // Prepare light data arrays
  std::vector<Eigen::Vector2f> light_positions;
  std::vector<Eigen::Vector3f> light_colors;
  std::vector<float> light_radii;

  light_positions.reserve(light_count);
  light_colors.reserve(light_count);
  light_radii.reserve(light_count);

  // Extract data from active lights
  for (int32_t i = 0; i < light_count; ++i) {
    const auto &light_info = active_lights_[i];

    // Position and radius (depends on geometry type)
    Eigen::Vector2f position;
    float radius = 1.0f; // Default fallback

    if (light_info.geometry) {
      const auto geometry_type = light_info.geometry->get_geometry_type();

      if (geometry_type == component::CircularLightGeometry::geometry_type_name) {
        // Regular circular light - use entity position
        position = light_info.world_position;
        const auto *circular_geometry =
            dynamic_cast<const component::CircularLightGeometry *>(
                light_info.geometry.get());
        if (circular_geometry) {
          radius = circular_geometry->get_radius();
        }
      } else if (geometry_type == component::GlobalLightGeometry::geometry_type_name) {
        // Global light - center at viewport with very large radius
        position = screen.get_viewport_center();
        radius = 1000.0f; // Very large radius to cover entire screen
      } else {
        // Unknown geometry type - fallback to entity position
        position = light_info.world_position;
      }
    } else {
      // No geometry - fallback to entity position
      position = light_info.world_position;
    }

    // Add processed data to arrays
    light_positions.push_back(position);
    light_radii.push_back(radius);

    // Color with intensity applied
    const float effective_intensity = light_info.intensity;
    light_colors.push_back(
        Eigen::Vector3f{light_info.color.r * effective_intensity,
                        light_info.color.g * effective_intensity,
                        light_info.color.b * effective_intensity});
  }

  // Set uniform arrays
  lighting_shader_->set_uniform_array("light_positions", light_positions);
  lighting_shader_->set_uniform_array("light_colors", light_colors);
  lighting_shader_->set_uniform_array("light_radii", light_radii);

  return Ok();
}

} // namespace systems
