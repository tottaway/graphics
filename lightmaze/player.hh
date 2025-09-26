#pragma once
#include "model/game_state.hh"
#include "view/texture.hh"
#include <Eigen/Geometry>

namespace component {
class Jumper;
class LightEmitter;
class LightMazeLightVolume;
} // namespace component

namespace lightmaze {

/**
 * @brief Player entity for the LightMaze game
 *
 * A basic platformer player character that can move horizontally and jump.
 * Uses the wiz player sprite for visual representation. This is a simplified
 * version that will be extended with lighting mechanics later.
 */
class Player : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "lightmaze_player";
  static constexpr std::string_view player_texture_set_path{
      "sprites/wiz/player/player_sprites.yaml"};

  /**
   * @brief Construct a new Player entity
   * @param game_state Reference to the game state that owns this entity
   */
  Player(model::GameState &game_state);

  /**
   * @brief Initialize the player with components and default state
   * @return Ok() on success, Err(message) if initialization fails
   * @pre Entity must be added to game state before calling init()
   * @post Player will have sprite, collision, and center components added
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
   * @brief Update player state including physics and movement
   * @param delta_time_ns Time elapsed since last update in nanoseconds
   * @return Ok() on success, Err(message) if update fails
   * @pre delta_time_ns should be positive
   * @post Player position and velocity updated based on input and physics
   */
  [[nodiscard]] virtual Result<void, std::string>
  update(const int64_t delta_time_ns);

  /**
   * @brief Handle key press events for player movement
   * @param key_press Key press event containing which key was pressed
   * @return true if event was handled and should not propagate, false otherwise
   * @post Movement flags updated based on arrow keys and spacebar
   */
  [[nodiscard]] virtual Result<bool, std::string>
  on_key_press(const view::KeyPressedEvent &key_press);

  /**
   * @brief Handle key release events for player movement
   * @param key_release Key release event containing which key was released
   * @return true if event was handled and should not propagate, false otherwise
   * @post Movement flags updated based on arrow keys
   */
  [[nodiscard]] virtual Result<bool, std::string>
  on_key_release(const view::KeyReleasedEvent &key_release);

  /**
   * @brief Get the current transform matrix for rendering
   * @return Affine transform representing position and scale
   * @post Transform reflects current position and size
   */
  [[nodiscard]] virtual Eigen::Affine2f get_transform() const;

  /**
   * @brief Get the z-level for rendering order
   * @return Z-level value (higher values render on top)
   */
  [[nodiscard]] virtual uint8_t get_z_level() const { return 1; }

private:
  /**
   * @brief Update player light color and refresh light components
   * @param new_color New RGB color for player light
   * @post Light emitter and light volume components updated with new color
   */
  void set_light_color(const view::Color &new_color);
  /// Current position in world coordinates (meters)
  Eigen::Vector2f position_{0.0f, 0.0f};

  /// Size of the player hitbox in meters
  Eigen::Vector2f size_{0.1f, 0.1f};

  /// Movement input state flags
  bool move_left_pressed_{false};
  bool move_right_pressed_{false};
  bool jump_pressed_{false};

  /// Current velocity in meters per second
  Eigen::Vector2f velocity_{0.0f, 0.0f};

  /// Horizontal movement speed in meters per second
  static constexpr float move_speed_{2.0f};

  /// Initial jump velocity in meters per second
  static constexpr float jump_speed_{4.0f};

  /// Gravity acceleration in meters per second squared (negative = downward)
  static constexpr float gravity_{-9.8f};

  /// Cached pointer to the Jumper component (safe because entities own their
  /// components)
  component::Jumper *jumper_component_{nullptr};

  /// Cached pointers to light components for color updates
  component::LightEmitter *light_emitter_component_{nullptr};
  component::LightMazeLightVolume *light_volume_component_{nullptr};

  /// Color of the player's light emission for LightMaze mechanics
  view::Color player_light_color_{255, 255, 255}; // Start with white
};

} // namespace lightmaze
