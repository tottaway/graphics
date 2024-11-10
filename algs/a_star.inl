#pragma once
#include <cstdint>
#include <limits>
#include <optional>
#include <queue>
#include <string>
#include <unordered_map>

namespace algs {

std::uint64_t get_hash_key_for_node(const Eigen::Vector2i &node) {
  return static_cast<uint64_t>(node.x()) +
         (static_cast<uint64_t>(node.y()) << sizeof(node.y()));
}

template <typename NodeType, std::size_t max_neighbors>
Result<void, std::string>
a_star(DistanceFunc<NodeType> distance_func,
       GetNeighborsFunc<NodeType, max_neighbors> get_neighbors,
       HueristicFunc<NodeType> hueristic_func, const NodeType &start,
       const NodeType &end, const std::size_t storage_size,
       std::optional<std::deque<NodeType>> &maybe_path) {

  std::unordered_map<uint64_t, NodeType> came_from;
  std::unordered_map<uint64_t, float> cost_so_far;

  using NodeCostPair = std::pair<float, NodeType>;
  std::vector<NodeCostPair> priority_queue_container;
  priority_queue_container.reserve(storage_size);
  const auto compare = [](const NodeCostPair &a, const NodeCostPair &b) {
   return a.first > b.first;
  };
  std::priority_queue<NodeCostPair, std::vector<NodeCostPair>,
                      decltype(compare)>
      frontier(compare, std::move(priority_queue_container));

  came_from.reserve(storage_size);
  cost_so_far.reserve(storage_size);

  const auto start_hash_key = get_hash_key_for_node(start);
  frontier.emplace(std::make_pair(0.f, start));
  cost_so_far.emplace(start_hash_key, 0.f);

  bool found_path = false;
  NodeCostPair best_node{hueristic_func(start), start};
  while (!frontier.empty()) {
    auto current = frontier.top().second;
    frontier.pop();

    if (current == end) {
      found_path = true;
      break;
    }

    const auto neighbors = get_neighbors(current);
    const auto current_hash_key = get_hash_key_for_node(current);
    const auto cost = cost_so_far.find(current_hash_key)->second;
    for (std::size_t i = 0; i < neighbors.num_neighbors; ++i) {
      const auto next = neighbors.neighbor_array.at(i);
      float new_cost = cost + distance_func(current, next);
      const auto hash_key = get_hash_key_for_node(next);
      const auto find_result = cost_so_far.find(hash_key);
      if (find_result == cost_so_far.end() || new_cost < find_result->second) {
        cost_so_far.emplace(hash_key, new_cost);
        float hueristic = hueristic_func(next);
        if (hueristic < best_node.first) {
          best_node = std::make_pair(hueristic, next);
        }
        float priority = new_cost + hueristic;
        frontier.emplace(std::make_pair(priority, next));
        came_from.emplace(hash_key, current);
      }
    }
    if (frontier.size() > storage_size || cost_so_far.size() > storage_size ||
        came_from.size() > storage_size) {
      return Err(
          std::string("Used too much space in a_star, increase estimate"));
    }
  }

  const auto node_to_reconstruct_path_from =
      found_path ? end : best_node.second;
  auto &path = maybe_path.emplace({node_to_reconstruct_path_from});
  auto current =
      came_from.find(get_hash_key_for_node(node_to_reconstruct_path_from));
  while (current != came_from.end()) {
    path.push_front(current->second);
    current = came_from.find(get_hash_key_for_node(current->second));
  }

  return Ok();
}

} // namespace algs
