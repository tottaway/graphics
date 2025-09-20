#include "lightmaze/mode_manager.hh"
#include "lightmaze/platform.hh"
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
  // Create ground platform at y=0 (top center at y=0, so surface is at y=0)
  auto ground_platform = std::make_unique<Platform>(
      game_state_, Eigen::Vector2f{0.0f, 0.0f}, // top center position
      Eigen::Vector2f{1.0f, 0.2f}               // size: 1m wide, 0.2m thick
  );
  TRY_VOID(ground_platform->init());
  TRY_VOID(game_state_.add_entity(std::move(ground_platform)));

  // Create second platform up and to the right for jump testing
  auto upper_platform = std::make_unique<Platform>(
      game_state_,
      Eigen::Vector2f{1.5f, .8f}, // top center position (right and up)
      Eigen::Vector2f{1.0f, 0.2f} // size: 1m wide, 0.2m thick
  );
  TRY_VOID(upper_platform->init());
  TRY_VOID(game_state_.add_entity(std::move(upper_platform)));

  // Create and initialize the player at the center of the screen
  auto player = std::make_unique<Player>(game_state_);
  TRY_VOID(player->init());
  TRY_VOID(game_state_.add_entity(std::move(player)));

  return Ok();
}

} // namespace lightmaze
