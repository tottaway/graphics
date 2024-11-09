#include "wiz/mode_manager.hh"
#include "model/game_state.hh"
#include "wiz/character_mode.hh"
#include "wiz/end_screen.hh"
#include "wiz/enemies/skeleton_spawner.hh"
#include "wiz/good_npcs/worker.hh"
#include "wiz/map/map.hh"
#include "wiz/movable_stone.hh"
#include "wiz/player.hh"

namespace wiz {
WizModeManager::WizModeManager(model::GameState &game_state)
    : model::Entity(game_state) {}

Result<void, std::string> WizModeManager::update(const int64_t delta_time_ns) {
  duration_in_mode_ns_ += delta_time_ns;
  switch (game_mode_) {
  case GameMode::init: {
    TRY_VOID(start_new_game());
    game_mode_ = GameMode::in_game;
    duration_in_mode_ns_ = 0L;
    break;
  }
  case GameMode::in_game: {
    const auto player = TRY(game_state_.get_entity_pointer_by_type<Player>());
    if (player->get_mode() == CharacterMode::dead) {
      remove_child_entities();
      TRY(add_child_entity_and_init<EndScreen>(
          GameResult{duration_in_mode_ns_}));
      game_mode_ = GameMode::dead;
      duration_in_mode_ns_ = 0L;
    }
    break;
  }
  case GameMode::dead: {
    const auto end_screen =
        TRY(game_state_.get_entity_pointer_by_type<EndScreen>());
    if (end_screen->has_been_clicked) {
      remove_child_entities();
      TRY_VOID(start_new_game());
      game_mode_ = GameMode::in_game;
      duration_in_mode_ns_ = 0L;
    }
  }
  }
  return Ok();
}

Result<void, std::string> WizModeManager::start_new_game() {
  TRY(add_child_entity_and_init<Map>());
  TRY(add_child_entity_and_init<Player>());
  TRY(add_child_entity_and_init<MovableStone>(Eigen::Vector2f{1.0f, 1.0f}));
  TRY(add_child_entity_and_init<SkeletonSpawner>(Eigen::Vector2f{1.5f, 1.5f}));
  TRY(add_child_entity_and_init<Worker>(Eigen::Vector2f{0.f, 0.f}));
  return Ok();
}
} // namespace wiz
