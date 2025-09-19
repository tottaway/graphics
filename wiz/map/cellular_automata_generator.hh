#pragma once
#include <array>
#include <random>
#include <vector>
#include <set>
#include <queue>
#include <iostream>

namespace wiz {

/**
 * @brief Generates maps using a cellular automata algorithm with guaranteed entity connectivity.
 *
 * This advanced map generator creates organic, cave-like structures while ensuring perfect
 * entity navigability. The algorithm combines cellular automata generation with sophisticated
 * post-processing to guarantee:
 * - All walkable areas have adequate 2x2 clearance for entity movement
 * - All regions are connected via traversable corridors
 * - Visual consistency where every walkable tile is actually reachable
 *
 * Algorithm Pipeline:
 * 1. Initialize with random noise (configurable wall probability)
 * 2. Apply cellular automata smoothing iterations to create organic structures
 * 3. Ensure border walls are maintained around map perimeter
 * 4. Widen narrow passages to guarantee 2x2 clearance areas
 * 5. Detect disconnected regions using 2x2 block connectivity analysis
 * 6. Create minimal corridors to connect isolated components
 * 7. Generate ASCII visualization for debugging
 *
 * Key Features:
 * - Entity-aware connectivity: Uses 2x2 block analysis instead of simple tile adjacency
 * - Minimal disruption: Only creates corridors where absolutely necessary
 * - Intelligent pathfinding: Connects closest blocks between disconnected regions
 * - Visual debugging: ASCII output shows final navigable structure
 *
 * @tparam MapSizeX Width of the map in tiles
 * @tparam MapSizeY Height of the map in tiles
 */
template<int64_t MapSizeX, int64_t MapSizeY>
class CellularAutomataGenerator {
public:
  static constexpr int64_t map_size_x = MapSizeX;
  static constexpr int64_t map_size_y = MapSizeY;

  using MapGrid = std::array<std::array<bool, map_size_x>, map_size_y>;

  /**
   * @brief Configuration parameters for cellular automata generation.
   */
  struct GenerationParams {
    /// Initial probability of a tile being a wall during noise generation [0.0, 1.0]
    float initial_wall_probability = 0.42f;
    /// Number of smoothing iterations to apply (more = smoother, rounder caves)
    int smoothing_iterations = 5;
    /// Minimum wall neighbors required for a tile to become/remain a wall
    int wall_neighbor_threshold = 4;
    /// Minimum passage width to ensure adequate entity clearance
    int min_passage_width = 2;
    /// Random seed for reproducible generation (0 = random seed)
    uint32_t seed = 0;
  };

  /**
   * @brief Constructs a cellular automata generator with the given parameters.
   * @param params Configuration for the generation algorithm
   */
  explicit CellularAutomataGenerator(const GenerationParams& params = {});

  /**
   * @brief Generates a complete map with guaranteed entity connectivity.
   *
   * Executes the full pipeline: noise generation, smoothing, passage widening,
   * connectivity analysis, and corridor creation. The resulting map ensures
   * all walkable areas are reachable and have adequate 2x2 clearance.
   *
   * @return MapGrid where true = wall, false = walkable
   * @post All walkable tiles are guaranteed to be reachable from each other
   * @post All walkable areas have adequate 2x2 clearance for entity movement
   */
  MapGrid generate();

  [[nodiscard]] bool is_wall(int x, int y) const;
  [[nodiscard]] const MapGrid& get_grid() const;
  [[nodiscard]] constexpr int64_t get_map_size_x() const { return map_size_x; }
  [[nodiscard]] constexpr int64_t get_map_size_y() const { return map_size_y; }

  /**
   * @brief Prints the map as ASCII art to stdout for debugging.
   *
   * Uses '#' for walls and '.' for walkable areas. Useful for visualizing
   * the generated map structure and debugging connectivity issues.
   */
  void print_map_ascii() const;

private:
  GenerationParams params_;
  std::mt19937 rng_;
  MapGrid current_grid_;

  MapGrid initialize_with_noise();
  void smooth_iteration(MapGrid& grid);
  int count_wall_neighbors(const MapGrid& grid, int x, int y);
  void ensure_border_walls(MapGrid& grid);
  void widen_narrow_passages(MapGrid& grid);
  bool is_narrow_passage(const MapGrid& grid, int x, int y) const;
  void ensure_connectivity(MapGrid& grid);
  std::vector<std::pair<int32_t, int32_t>> find_valid_2x2_blocks(const MapGrid& grid) const;
  bool blocks_are_connected(const MapGrid& grid, int32_t x1, int32_t y1, int32_t x2, int32_t y2) const;
  bool is_valid_entity_move(const MapGrid& grid, int32_t from_x, int32_t from_y, int32_t to_x, int32_t to_y) const;
  void create_connecting_corridor(MapGrid& grid, const std::vector<std::pair<int32_t, int32_t>>& from_component, const std::vector<std::pair<int32_t, int32_t>>& to_component);
  std::pair<std::pair<int32_t, int32_t>, std::pair<int32_t, int32_t>> find_closest_blocks(const std::vector<std::pair<int32_t, int32_t>>& component1, const std::vector<std::pair<int32_t, int32_t>>& component2) const;
};

} // namespace wiz

#include "wiz/map/cellular_automata_generator.inl"