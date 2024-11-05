#include "wiz/mode_manager.hh"
#include "model/game_state.hh"
#include "snake/snake.hh"
#include "wiz/enemies/skeleton_spawner.hh"
#include "wiz/map/map.hh"
#include "wiz/movable_stone.hh"
#include "wiz/player.hh"

namespace wiz {
WizModeManager::WizModeManager(model::GameState &game_state)
    : model::Entity(game_state) {}

Result<void, std::string> WizModeManager::update(const int64_t delta_time_ns) {
  switch (game_mode_) {
  case GameMode::init: {
    TRY_VOID(start_new_game());
    game_mode_ = GameMode::in_game;
    break;
  }
  case GameMode::in_game: {
    const auto player = TRY(game_state_.get_entity_pointer_by_type<Player>());
    if (player->hp == 0) {
      remove_child_entities();
      TRY(add_child_entity_and_init<snake::EndScreen>(snake::GameResult{10}));
      game_mode_ = GameMode::dead;
    }
    break;
  }
  }
  return Ok();
}

Result<void, std::string> WizModeManager::start_new_game() {
  TRY(add_child_entity_and_init<Map>());
  TRY(add_child_entity_and_init<Player>());
  TRY(add_child_entity_and_init<MovableStone>(Eigen::Vector2f{1.0f, 1.0f}));
  TRY(add_child_entity_and_init<SkeletonSpawner>(Eigen::Vector2f{1.5f, 1.5f}));
  return Ok();
}
} // namespace wiz
