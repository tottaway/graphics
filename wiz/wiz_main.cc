#include "controller/controller.hh"
#include "view/screen.hh"
#include "wiz/wiz.hh"
#include <chrono>
#include <cstdlib>

int main() {
  auto game_result = wiz::make_wiz_game();
  if (game_result.isErr()) {
    std::cerr << "Failed to initialize game with error: "
              << game_result.unwrapErr() << std::endl;
    return EXIT_FAILURE;
  }
  controller::Controller controller(std::make_unique<view::Screen>(),
                                    std::move(game_result).unwrap());
  const auto result = controller.run();
  if (result.isErr()) {
    std::cerr << "Application ended with error: " << result.unwrapErr()
              << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
