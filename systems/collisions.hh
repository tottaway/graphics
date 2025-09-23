#pragma once
#include "model/game_state.hh"
#include "systems/system.hh"

namespace systems {
class Collisions : public System {
public:
  static constexpr std::string_view system_type_name = "collisions_system";
  Collisions() = default;

  virtual ~Collisions() = default;

  virtual Result<void, std::string> update(model::GameState& game_state, const int64_t delta_time_ns) final;

  virtual std::string_view get_system_type_name() const final {
    return system_type_name;
  }

private:
};
} // namespace systems
