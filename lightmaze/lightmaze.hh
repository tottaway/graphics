#pragma once
#include "model/game_state.hh"
#include "utility/try.hh"

namespace lightmaze {

[[nodiscard]] Result<std::unique_ptr<model::GameState>, std::string>
make_lightmaze_game();

}