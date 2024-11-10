#pragma once
#include "utility/try.hh"
#include <deque>
#include <string>
#include <vector>

namespace algs {

template <typename NodeType>
using DistanceFunc = std::function<float(const NodeType &, const NodeType &)>;

template <typename NodeType>
using HueristicFunc = std::function<float(const NodeType &)>;

template <typename NodeType, std::size_t max_neighbors> struct Neighbors {
  std::array<NodeType, max_neighbors> neighbor_array;
  std::size_t num_neighbors{0UL};
};

template <typename NodeType, std::size_t max_neighbors>
using GetNeighborsFunc =
    std::function<Neighbors<NodeType, max_neighbors>(const NodeType &)>;

template <typename NodeType, std::size_t max_neighbors>
Result<void, std::string>
a_star(DistanceFunc<NodeType> distance_func,
       GetNeighborsFunc<NodeType, max_neighbors> get_neighbors,
       HueristicFunc<NodeType> hueristic_func, const NodeType &start,
       const NodeType &end, const std::size_t storage_size,
       std::optional<std::deque<NodeType>> &maybe_path);

} // namespace algs
#include "algs/a_star.inl"
