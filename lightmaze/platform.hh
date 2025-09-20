#pragma once
#include "model/game_state.hh"
#include <Eigen/Geometry>

namespace lightmaze {

/**
 * @brief Static platform entity that provides collision surfaces for platformer gameplay
 *
 * Creates a white rectangular platform that can be positioned anywhere in the world.
 * Provides both visual reference for player movement and collision detection for
 * platformer physics. Multiple platforms can be used to create complex level layouts.
 */
class Platform : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "lightmaze_platform";

  /**
   * @brief Construct a new Platform entity
   * @param game_state Reference to the game state that owns this entity
   * @param top_center_position Position of the platform's top center in world coordinates
   * @param size Size of the platform in meters (width, height)
   * @post Platform will be positioned so its top center is at top_center_position
   */
  Platform(model::GameState &game_state, const Eigen::Vector2f &top_center_position = Eigen::Vector2f{0.0f, 0.0f},
           const Eigen::Vector2f &size = Eigen::Vector2f{2.0f, 0.2f});

  /**
   * @brief Initialize the platform with visual and collision components
   * @return Ok() on success, Err(message) if initialization fails
   * @pre Entity must be added to game state before calling init()
   * @post Platform will have rectangle drawing and collision components added
   */
  Result<void, std::string> init();

  /**
   * @brief Get the entity type name for identification
   * @return Static string identifying this entity type
   */
  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  };

  /**
   * @brief Update platform state (currently no-op since platform is static)
   * @param delta_time_ns Time elapsed since last update in nanoseconds
   * @return Ok() always succeeds for static platform
   */
  [[nodiscard]] virtual Result<void, std::string>
  update(const int64_t delta_time_ns);

  /**
   * @brief Get the current transform matrix for rendering
   * @return Affine transform representing position and scale
   * @post Transform reflects platform position and size
   */
  [[nodiscard]] virtual Eigen::Affine2f get_transform() const;

  /**
   * @brief Get the z-level for rendering order (background)
   * @return Z-level value (lower values render behind)
   */
  [[nodiscard]] virtual uint8_t get_z_level() const { return 0; }

  /**
   * @brief Get the top center position of the platform
   * @return World coordinates of the platform's top center point
   * @post Useful for jump calculations and platform placement
   */
  [[nodiscard]] Eigen::Vector2f get_top_center_position() const;

private:
  /// Platform position in world coordinates
  Eigen::Vector2f position_;

  /// Platform dimensions in meters
  Eigen::Vector2f size_;
};

} // namespace lightmaze
