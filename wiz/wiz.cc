#include "wiz/wiz.hh"
#include "wiz/mode_manager.hh"

namespace wiz {
[[nodiscard]] Result<std::unique_ptr<model::GameState>, std::string>
make_wiz_game() {
  auto game_state = std::make_unique<model::GameState>();
  TRY(game_state->add_entity(std::make_unique<WizModeManager>(*game_state)));
  return Ok(std::move(game_state));
}

} // namespace wiz
