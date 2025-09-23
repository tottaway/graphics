#pragma once
#include "components/light_emitter.hh"
#include "model/game_state.hh"
#include "systems/system.hh"
#include "view/screen.hh"
#include <vector>

namespace systems {

/**
 * @brief System that manages all lighting in the game
 *
 * The LightingSystem collects all LightEmitter components during update
 * and renders a lighting overlay during draw. It implements a "black by default"
 * lighting model where only illuminated areas are visible.
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

  /**
   * @brief Render a single circular light to the screen
   * @param screen Screen to render to
   * @param light_info Information about the light to render
   * @return Ok() on success, Err(message) if rendering fails
   */
  Result<void, std::string> render_circular_light(
      view::Screen& screen,
      const component::LightEmitter::LightInfo& light_info) const;
};

} // namespace systems