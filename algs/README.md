# Algorithms Module

The algorithms module provides reusable algorithmic implementations for common game development needs.

## Pathfinding

### A* Algorithm (`a_star.hh`, `a_star.inl`)

Template-based implementation of the A* pathfinding algorithm with configurable node types and neighbor constraints.

**Key features:**
- Generic node type support (typically `Eigen::Vector2i` for grid-based maps)
- Configurable maximum neighbors per node
- Memory-bounded execution with storage size limits
- Fallback to best partial path when target unreachable
- Hash-based node tracking for efficient lookups

**Function Signature:**
```cpp
template <typename NodeType, std::size_t max_neighbors>
Result<void, std::string>
a_star(DistanceFunc<NodeType> distance_func,
       GetNeighborsFunc<NodeType, max_neighbors> get_neighbors,
       HueristicFunc<NodeType> hueristic_func,
       const NodeType &start,
       const NodeType &end,
       const std::size_t storage_size,
       std::optional<std::deque<NodeType>> &maybe_path);
```

**Type Definitions:**
```cpp
// Distance between two adjacent nodes
template <typename NodeType>
using DistanceFunc = std::function<float(const NodeType &, const NodeType &)>;

// Estimated distance to goal (heuristic)
template <typename NodeType>
using HueristicFunc = std::function<float(const NodeType &)>;

// Get neighbors for a given node
template <typename NodeType, std::size_t max_neighbors>
using GetNeighborsFunc =
    std::function<Neighbors<NodeType, max_neighbors>(const NodeType &)>;
```

**Usage Example:**
```cpp
// Define node type and maximum neighbors
using NodeType = Eigen::Vector2i;
constexpr std::size_t max_neighbors = 8;  // 8-directional movement

// Define distance function (Manhattan distance)
auto distance_func = [](const NodeType &a, const NodeType &b) -> float {
    return (a - b).cwiseAbs().sum();
};

// Define heuristic function (distance to goal)
NodeType goal{10, 10};
auto heuristic_func = [goal](const NodeType &node) -> float {
    return (node - goal).cwiseAbs().sum();
};

// Define neighbor function (8-directional grid)
auto get_neighbors = [](const NodeType &node) -> Neighbors<NodeType, max_neighbors> {
    Neighbors<NodeType, max_neighbors> neighbors;

    // Add all 8 directions
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;  // Skip self

            NodeType neighbor = node + NodeType{dx, dy};
            // Add validation logic here (bounds checking, obstacle checking)
            neighbors.neighbor_array[neighbors.num_neighbors++] = neighbor;
        }
    }
    return neighbors;
};

// Run A* pathfinding
NodeType start{0, 0};
NodeType end{10, 10};
std::size_t storage_size = 1000;
std::optional<std::deque<NodeType>> maybe_path;

auto result = algs::a_star<NodeType, max_neighbors>(
    distance_func, get_neighbors, heuristic_func,
    start, end, storage_size, maybe_path);

if (result.isOk() && maybe_path.has_value()) {
    // Path found, use maybe_path.value()
    for (const auto& node : maybe_path.value()) {
        // Process path node
    }
}
```

**Implementation Details:**
- Uses a priority queue (min-heap) for the frontier
- Tracks came_from relationships for path reconstruction
- Maintains cost_so_far for each visited node
- Hash function optimized for `Eigen::Vector2i` nodes
- Bounded by storage_size to prevent excessive memory usage
- Returns best partial path if full path not found

**Performance Considerations:**
- Reserve memory upfront using `storage_size` parameter
- Hash-based lookups provide O(1) average-case access
- Memory usage scales with explored nodes, not grid size
- Early termination when storage limits exceeded

**Error Handling:**
- Returns `Err()` if storage size exceeded during search
- Returns `Ok()` with partial path if target unreachable
- Path reconstruction always succeeds if any nodes were visited

## Example Integration

See the wiz game for pathfinding usage in AI entities:
- Worker NPCs use A* to navigate around obstacles
- Skeleton enemies pathfind to player positions
- Map-based neighbor functions respect terrain constraints

## Extending the Module

The template-based design allows easy extension:
- Custom node types (3D coordinates, waypoints, etc.)
- Different neighbor patterns (4-directional, hexagonal grids)
- Custom distance functions (Euclidean, weighted movement)
- Application-specific heuristics