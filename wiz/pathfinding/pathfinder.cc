#include "wiz/pathfinding/pathfinder.hh"
#include "algs/a_star.hh"
#include "wiz/map/map.hh"

namespace wiz {
namespace pathfinding {

Result<std::deque<Eigen::Vector2i>, std::string> find_path(
    model::GameState& game_state,
    const Eigen::Vector2f& start_position,
    const Eigen::Vector2f& goal_position,
    MapInteractionType movement_type,
    size_t max_nodes_to_explore) {

  const auto map = TRY(game_state.get_entity_pointer_by_type<Map>());
  const auto start_tile = map->get_tile_index_by_position(start_position);
  const auto goal_tile = map->get_tile_index_by_position(goal_position);

  auto distance_func = [](const Eigen::Vector2i &tile_a,
                          const Eigen::Vector2i &tile_b) {
    return (tile_a - tile_b).norm();
  };

  auto get_neighbors = [&map, movement_type](const Eigen::Vector2i &tile_index) {
    std::array<Eigen::Vector2i, 4> side_neighbors_deltas{
        Eigen::Vector2i{0, 1},  Eigen::Vector2i{0, -1}, Eigen::Vector2i{1, 0},
        Eigen::Vector2i{-1, 0}
    };

    std::array<Eigen::Vector2i, 4> diagonal_neighbors_deltas{
        Eigen::Vector2i{1, 1},  Eigen::Vector2i{-1, -1},
        Eigen::Vector2i{-1, 1}, Eigen::Vector2i{1, -1}
    };

    algs::Neighbors<Eigen::Vector2i, 8> neighbors;

    // Add cardinal direction neighbors
    for (const auto &neighbor_delta : side_neighbors_deltas) {
      const auto neighbor_tile_index = tile_index + neighbor_delta;
      if (map->is_walkable_tile(neighbor_tile_index, movement_type)) {
        neighbors.neighbor_array[neighbors.num_neighbors++] = neighbor_tile_index;
      }
    }

    // Add diagonal neighbors only if both adjacent cardinal directions are walkable
    for (const auto &neighbor_delta : diagonal_neighbors_deltas) {
      const auto neighbor_tile_index = tile_index + neighbor_delta;
      const Eigen::Vector2i adjacent_side_tile1 = {neighbor_tile_index.x(), tile_index.y()};
      const Eigen::Vector2i adjacent_side_tile2 = {tile_index.x(), neighbor_tile_index.y()};

      uint8_t num_adjacent_side_tiles_included{0U};
      for (size_t i = 0; i < neighbors.num_neighbors; ++i) {
        const auto existing_neighbor = neighbors.neighbor_array[i];
        if (adjacent_side_tile1 == existing_neighbor ||
            adjacent_side_tile2 == existing_neighbor) {
          num_adjacent_side_tiles_included++;
        }
      }

      // Only allow diagonal movement if both adjacent cardinal directions are walkable
      if (num_adjacent_side_tiles_included == 2 &&
          map->is_walkable_tile(neighbor_tile_index, movement_type)) {
        neighbors.neighbor_array[neighbors.num_neighbors++] = neighbor_tile_index;
      }
    }

    return neighbors;
  };

  auto heuristic_function = [goal_tile](const Eigen::Vector2i &tile_index) {
    return (tile_index - goal_tile).norm();
  };

  std::optional<std::deque<Eigen::Vector2i>> maybe_path;
  TRY_VOID(algs::a_star<Eigen::Vector2i, 8>(
      distance_func, get_neighbors, heuristic_function, start_tile, goal_tile,
      max_nodes_to_explore, maybe_path));

  if (!maybe_path.has_value()) {
    return Err(std::string("No path found from start to goal"));
  }

  return Ok(std::move(maybe_path.value()));
}

} // namespace pathfinding
} // namespace wiz