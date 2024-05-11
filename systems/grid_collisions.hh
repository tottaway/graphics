#pragma once
#include "model/game_state.hh"
#include "systems/system.hh"

namespace systems {
template <std::size_t x_dim, std::size_t y_dim>
class GridCollisions : public System {
public:
  static constexpr std::string_view system_type_name = "grid_collisions_system";
  GridCollisions(model::GameState &game_state);

  virtual ~GridCollisions() = default;

  virtual Result<void, std::string> update(const int64_t delta_time_ns) final;

  virtual std::string_view get_system_type_name() const final {
    return system_type_name;
  }

private:
  model::GameState &game_state_;
};
} // namespace systems

#include "systems/grid_collisions.inl" // IWYU pragma: export
