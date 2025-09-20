#pragma once
#include "model/game_state.hh"
#include "utility/try.hh"

namespace lightmaze {

/**
 * @brief Mode manager entity that handles game state transitions for LightMaze
 *
 * Manages the overall game flow including initialization, gameplay, and
 * potential future states like pause/menu. Currently focuses on spawning
 * and managing the player entity.
 */
class LightMazeModeManager : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "lightmaze_mode_manager";

  /**
   * @brief Construct a new LightMaze Mode Manager
   * @param game_state Reference to the game state that owns this entity
   */
  LightMazeModeManager(model::GameState &game_state);

  /**
   * @brief Destructor
   */
  virtual ~LightMazeModeManager() = default;

  /**
   * @brief Update the mode manager state and handle transitions
   * @param delta_time_ns Time elapsed since last update in nanoseconds
   * @return Ok() on success, Err(message) if update fails
   * @pre delta_time_ns should be positive
   * @post Game state may be updated based on current mode
   */
  [[nodiscard]] virtual Result<void, std::string>
  update(const int64_t delta_time_ns);

  /**
   * @brief Get the entity type name for identification
   * @return Static string identifying this entity type
   */
  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  };

private:
  /**
   * @brief Initialize a new game by spawning the player
   * @return Ok() on success, Err(message) if player creation fails
   * @post Player entity added to game state at center of screen
   */
  [[nodiscard]] Result<void, std::string> start_new_game();

  /// Current game mode state
  enum class GameMode { init, in_game };
  GameMode game_mode_{GameMode::init};

  /// Time spent in the current mode (for potential transitions)
  int64_t duration_in_mode_ns_{0L};
};

} // namespace lightmaze