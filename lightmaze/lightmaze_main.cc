#include "controller/controller.hh"
#include "view/screen.hh"
#include "lightmaze/lightmaze.hh"
#include <chrono>
#include <cstdlib>
#include <iostream>

int main() {
  auto game_result = lightmaze::make_lightmaze_game();
  if (game_result.isErr()) {
    std::cerr << "Failed to initialize game with error: "
              << game_result.unwrapErr() << std::endl;
    return EXIT_FAILURE;
  }

  // Use a 4x3 viewport for platformer aspect ratio
  controller::Controller controller(
      std::make_unique<view::Screen>(Eigen::Vector2f{4.0f, 3.0f}),
      std::move(game_result).unwrap());

  const auto result = controller.run();
  if (result.isErr()) {
    std::cerr << "Application ended with error: " << result.unwrapErr()
              << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}