#pragma once
#include "components/grid_collider.hh"
#include "systems/grid_collisions.hh"
#include <Eigen/Dense>

namespace systems {

template <std::size_t x_dim, std::size_t y_dim>
Result<void, std::string>
GridCollisions<x_dim, y_dim>::update(model::GameState& game_state, const int64_t delta_time_ns) {
  std::array<std::array<std::vector<model::Entity *>, y_dim * 2 + 1>,
             x_dim * 2 + 1>
      occupancy_grid_;
  const auto entities =
      game_state.get_entities_with_component<component::GridCollider>();
  for (const auto entity : entities) {
    const auto collider =
        entity->get_component<component::GridCollider>().value();
    const auto cells = collider->get_cells();
    for (const auto &cell : cells) {
      auto &entities_in_cell =
          occupancy_grid_[cell.x() + x_dim][cell.y() + y_dim];
      for (const auto other_entity : entities_in_cell) {
        if (other_entity) {
          collider->handle_collision(other_entity->get_entity_id());
        }
      }
      entities_in_cell.emplace_back(entity);
    }
  }
  return Ok();
}
} // namespace systems
