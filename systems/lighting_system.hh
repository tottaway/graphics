#pragma once
#include "components/light_emitter.hh"
#include "model/game_state.hh"
#include "systems/system.hh"
#include "view/screen.hh"
#include "view/shader.hh"
#include <vector>
#include <memory>

namespace systems {

/**
 * @brief System that manages all lighting in the game
 *
 * The LightingSystem collects all LightEmitter components during update
 * and renders a shader-based lighting overlay during draw. It implements a "black by default"
 * lighting model where only illuminated areas are visible using GLSL shaders for realistic
 * lighting effects with smooth falloff and multiple light support.
 */
class LightingSystem : public System {
public:
  /**
   * @brief Construct a new LightingSystem
   */
  LightingSystem() = default;

  /**
   * @brief Get the system type name for identification
   * @return Static string identifying this system type
   */
  [[nodiscard]] virtual std::string_view get_system_type_name() const override {
    return "lighting_system";
  }

  /**
   * @brief Update the lighting system by collecting all active lights
   * @param game_state Game state containing all entities with LightEmitter components
   * @param delta_time_ns Time elapsed since last update in nanoseconds
   * @return Ok() on success, Err(message) if collection fails
   * @post All active lights collected for rendering
   */
  [[nodiscard]] virtual Result<void, std::string>
  update(model::GameState& game_state, const int64_t delta_time_ns) override;

  /**
   * @brief Draw the lighting overlay to the screen
   * @param screen Screen to render lighting effects to
   * @return Ok() on success, Err(message) if rendering fails
   * @post Lighting overlay rendered based on collected light sources
   */
  [[nodiscard]] virtual Result<void, std::string>
  draw(view::Screen& screen) override;

private:
  /// Collected light information for current frame
  std::vector<component::LightEmitter::LightInfo> active_lights_;

  /// Lighting shader for rendering all lights at once
  std::unique_ptr<view::Shader> lighting_shader_;

  /// Flag to track shader loading attempts
  bool shader_load_attempted_{false};

  /// Shader file paths
  static constexpr std::string_view vertex_shader_path_ = "systems/assets/shaders/lighting.vert";
  static constexpr std::string_view fragment_shader_path_ = "systems/assets/shaders/lighting.frag";

  /**
   * @brief Ensure lighting shader is loaded (lazy loading)
   * @return Ok() on success, Err(message) if shader loading fails
   */
  Result<void, std::string> ensure_shader_loaded();

  /**
   * @brief Set shader uniforms based on collected lights and screen viewport
   * @param screen Screen to get viewport information from
   * @return Ok() on success, Err(message) if uniform setting fails
   */
  Result<void, std::string> set_lighting_uniforms(const view::Screen& screen) const;
};

} // namespace systems