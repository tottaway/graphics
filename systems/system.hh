#pragma once
#include "utility/try.hh"

namespace view {
class Screen;
}

namespace model {
class GameState;
}

namespace systems {
class System {
public:
  virtual ~System() = default;

  virtual Result<void, std::string> update(model::GameState& game_state, const int64_t delta_time_ns) = 0;

  virtual Result<void, std::string> draw(view::Screen& screen) {
    // Default implementation does nothing
    return Ok();
  }

  virtual std::string_view get_system_type_name() const = 0;
};
} // namespace systems
