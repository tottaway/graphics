#include "controller/controller.hh"
#include "utility/overload.hh"
#include "utility/try.hh"
#include <SFML/Window/Event.hpp>
#include <chrono>
#include <thread>
using namespace std::chrono_literals;

namespace controller {
Controller::Controller(std::unique_ptr<view::Screen> screen,
                       std::unique_ptr<model::GameState> game_state)
    : screen_(std::move(screen)), game_state_(std::move(game_state)) {}

Result<void, std::string>
Controller::run(std::chrono::milliseconds min_update_interval) {
  return run(std::make_optional(min_update_interval));
}

Result<void, std::string> Controller::run() { return run(std::nullopt); }

Result<void, std::string> Controller::run(
    std::optional<std::chrono::milliseconds> maybe_min_update_interval) {
  while (true) {
    const auto start = std::chrono::high_resolution_clock::now();
    screen_->start_update();
    if (!TRY(screen_->poll_events_and_check_for_close())) {
      return Ok();
    }

    for (const auto &event : screen_->get_events()) {
      TRY_VOID(game_state_->handle_event(event, *screen_));
    }

    TRY_VOID(game_state_->advance_state(100.f));
    TRY_VOID(game_state_->draw(*screen_));
    screen_->finish_update();
    screen_->clear_events();
    const auto end = std::chrono::high_resolution_clock::now();
    if (maybe_min_update_interval) {
      std::this_thread::sleep_for(maybe_min_update_interval.value() -
                                  (end - start));
    }
  }
  return Ok();
}
} // namespace controller
