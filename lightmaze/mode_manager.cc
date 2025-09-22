#include "lightmaze/mode_manager.hh"
#include "lightmaze/map/map.hh"
#include "lightmaze/player.hh"
#include "model/game_state.hh"

namespace lightmaze {

LightMazeModeManager::LightMazeModeManager(model::GameState &game_state)
    : model::Entity(game_state) {}

Result<void, std::string>
LightMazeModeManager::update(const int64_t delta_time_ns) {
  duration_in_mode_ns_ += delta_time_ns;

  switch (game_mode_) {
  case GameMode::init:
    // Start the game immediately
    TRY_VOID(start_new_game());
    game_mode_ = GameMode::in_game;
    duration_in_mode_ns_ = 0L;
    break;

  case GameMode::in_game:
    // Game is running, no mode transitions for now
    break;
  }

  return Ok();
}

Result<void, std::string> LightMazeModeManager::start_new_game() {
  // Create the Map entity which will handle platforms and editor mode
  TRY(add_child_entity_and_init<Map>());

  // Create and initialize the player at the center of the screen
  TRY(add_child_entity_and_init<Player>());

  return Ok();
}

} // namespace lightmaze
