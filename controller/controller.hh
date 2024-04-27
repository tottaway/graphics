#pragma once
#include "model/game_state.hh"
#include "utility/try.hh"
#include "view/screen.hh"
#include <chrono>
#include <memory>

namespace controller {
class Controller {
public:
  Controller(std::unique_ptr<view::Screen> screen,
             std::unique_ptr<model::GameState> game_state);

  Result<void, std::string> run();
  Result<void, std::string> run(std::chrono::milliseconds min_update_interval);

private:
  Result<void, std::string>
  run(std::optional<std::chrono::milliseconds> maybe_min_update_interval);
  std::unique_ptr<view::Screen> screen_;
  std::unique_ptr<model::GameState> game_state_;
};
} // namespace controller
