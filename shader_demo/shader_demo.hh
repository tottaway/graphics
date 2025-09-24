#pragma once
#include "model/game_state.hh"
#include "utility/try.hh"
#include <memory>

namespace shader_demo {

/**
 * @brief Create a shader demo game state for testing shader functionality
 *
 * This creates a minimal game with test entities that demonstrate:
 * - Basic shader loading and compilation
 * - Simple lighting effects
 * - Animated shader parameters
 *
 * @return Game state ready for use with controller, or error message
 */
[[nodiscard]] Result<std::unique_ptr<model::GameState>, std::string>
make_shader_demo_game();

} // namespace shader_demo