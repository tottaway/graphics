#include "controller/controller.hh"
#include "snake/snake.hh"
#include "view/screen.hh"
#include <chrono>
#include <cstdlib>

int main() {
  auto game_result = snake::make_snake_game();
  if (game_result.isErr()) {
    std::cerr << "Failed to initialize game with error: "
              << game_result.unwrapErr() << std::endl;
    return EXIT_FAILURE;
  }
  controller::Controller controller(std::make_unique<view::Screen>(),
                                    std::move(game_result).unwrap());
  const auto result = controller.run(std::chrono::milliseconds(250));
  if (result.isErr()) {
    std::cerr << "Application ended with error: " << result.unwrapErr()
              << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
