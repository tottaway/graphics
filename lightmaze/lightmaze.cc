#include "lightmaze/lightmaze.hh"
#include "systems/collisions.hh"
#include "lightmaze/mode_manager.hh"

namespace lightmaze {
[[nodiscard]] Result<std::unique_ptr<model::GameState>, std::string>
make_lightmaze_game() {
  auto game_state = std::make_unique<model::GameState>();
  TRY(game_state->add_entity(std::make_unique<LightMazeModeManager>(*game_state)));
  game_state->add_system<systems::Collisions>();
  return Ok(std::move(game_state));
}

} // namespace lightmaze