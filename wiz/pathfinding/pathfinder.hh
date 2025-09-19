#pragma once
#include "model/game_state.hh"
#include "wiz/map/map.hh"
#include <deque>

namespace wiz {

/**
 * @brief Stateless A* pathfinding utilities for map navigation.
 *
 * This library provides pure functions for calculating paths on the map using A*.
 * It does not maintain any state - callers are responsible for caching paths,
 * replanning logic, and movement state management.
 */
namespace pathfinding {

/**
 * @brief Calculates an A* path from start to goal position.
 *
 * @param game_state Reference to game state for accessing the map
 * @param start_position Starting position in world coordinates
 * @param goal_position Goal position in world coordinates
 * @param movement_type Type of tiles this entity can walk on
 * @param max_nodes_to_explore Maximum nodes to explore before giving up (default 500)
 * @return Ok(path) containing tile indices from start to goal, or Err(message) if no path found
 * @post If successful, returned path contains at least start tile
 * @post Path tiles are valid and walkable for the given movement type
 */
Result<std::deque<Eigen::Vector2i>, std::string> find_path(
    model::GameState& game_state,
    const Eigen::Vector2f& start_position,
    const Eigen::Vector2f& goal_position,
    MapInteractionType movement_type,
    size_t max_nodes_to_explore = 500);

} // namespace pathfinding
} // namespace wiz