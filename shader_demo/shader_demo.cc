#include "shader_demo/shader_demo.hh"
#include "shader_demo/test_entity.hh"
#include <iostream>

namespace shader_demo {

Result<std::unique_ptr<model::GameState>, std::string> make_shader_demo_game() {
  std::cout << "Creating shader demo game..." << std::endl;

  auto game_state = std::make_unique<model::GameState>();

  // Create a test entity with a shader renderer component at origin
  std::cout << "Adding test entity..." << std::endl;

  const auto test_entity_result = game_state->add_entity_and_init<TestEntity>(Eigen::Vector2f{0.0f, 0.0f});
  if (test_entity_result.isErr()) {
    return Err(std::string("Failed to create test entity: ") + test_entity_result.unwrapErr());
  }

  std::cout << "Shader demo game created successfully!" << std::endl;
  return Ok(std::move(game_state));
}

} // namespace shader_demo