#include "shader_demo/shader_demo.hh"
#include "controller/controller.hh"
#include <iostream>

int main() {
  std::cout << "Starting Shader Demo..." << std::endl;

  // Create the shader demo game
  auto game_result = shader_demo::make_shader_demo_game();
  if (game_result.isErr()) {
    std::cerr << "Failed to initialize shader demo: " << game_result.unwrapErr() << std::endl;
    return EXIT_FAILURE;
  }

  // Create screen with reasonable viewport for testing
  auto screen = std::make_unique<view::Screen>(
    Eigen::Vector2f{4.0f, 4.0f},  // 4x4 meter viewport
    Eigen::Vector2f{0.0f, 0.0f}   // centered at origin
  );

  // Run the game loop
  controller::Controller controller(std::move(screen), std::move(game_result).unwrap());
  const auto result = controller.run();

  if (result.isErr()) {
    std::cerr << "Shader demo ended with error: " << result.unwrapErr() << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "Shader demo completed successfully." << std::endl;
  return EXIT_SUCCESS;
}