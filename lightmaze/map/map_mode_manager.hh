#pragma once
#include "model/game_state.hh"
#include "view/screen.hh"

namespace lightmaze {

/**
 * @brief Manages editor mode state and handles E key toggle
 *
 * The MapModeManager is a lightweight entity that handles editor mode state
 * and the E key toggle functionality. It provides a clean interface for
 * other entities to query the current editor mode without creating circular
 * dependencies.
 */
class MapModeManager : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "lightmaze_map_mode_manager";

  enum class EditorMode { gameplay, editor };

  /**
   * @brief Construct a new MapModeManager entity
   * @param game_state Reference to the game state that owns this entity
   */
  MapModeManager(model::GameState &game_state);

  /**
   * @brief Initialize the mode manager
   * @return Ok() on success, Err(message) if initialization fails
   */
  Result<void, std::string> init();

  /**
   * @brief Get the entity type name for identification
   * @return Static string identifying this entity type
   */
  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  }

  /**
   * @brief Check if currently in editor mode
   * @return true if in editor mode, false if in gameplay mode
   */
  bool is_editor_mode() const { return current_mode_ == EditorMode::editor; }

  /**
   * @brief Toggle between gameplay and editor modes
   * @return Ok() on success, Err(message) if toggle fails
   * @post Editor mode state flipped
   */
  Result<void, std::string> toggle_editor_mode();

  /**
   * @brief Handle key press events for editor mode toggle
   * @param key_press Key press event containing which key was pressed
   * @return false if E key was handled, true otherwise (continue processing)
   * @post Editor mode may be toggled if E key was pressed
   */
  [[nodiscard]] virtual Result<bool, std::string>
  on_key_press(const view::KeyPressedEvent &key_press);

private:
  /// Current editor mode state
  EditorMode current_mode_{EditorMode::gameplay};
};

} // namespace lightmaze