#pragma once
#include "model/game_state.hh"

namespace wiz {

[[nodiscard]] Result<std::unique_ptr<model::GameState>, std::string>
make_wiz_game();

}
