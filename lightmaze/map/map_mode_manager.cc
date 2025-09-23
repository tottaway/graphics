#include "lightmaze/map/map_mode_manager.hh"
#include "model/game_state.hh"
#include "view/screen.hh"
#include <SFML/Window/Keyboard.hpp>

namespace lightmaze {

MapModeManager::MapModeManager(model::GameState &game_state)
    : model::Entity(game_state) {}

Result<void, std::string> MapModeManager::init() {
  return Ok();
}

Result<void, std::string> MapModeManager::toggle_editor_mode() {
  if (current_mode_ == EditorMode::gameplay) {
    current_mode_ = EditorMode::editor;
  } else {
    current_mode_ = EditorMode::gameplay;
  }

  return Ok();
}

Result<bool, std::string>
MapModeManager::on_key_press(const view::KeyPressedEvent &key_press) {
  // TODO: Abstract key codes for portability - SFML types should be abstracted by view classes
  // Handle E key for editor mode toggle
  if (key_press.key_event.code == sf::Keyboard::E) {
    TRY_VOID(toggle_editor_mode());
    return Ok(false); // Event handled, stop processing other entities
  }

  return Ok(true); // Event not handled, continue processing other entities
}

} // namespace lightmaze