#pragma once
#include <array>
#include <random>
#include <vector>

namespace wiz {

/**
 * @brief Generates maps using a room-and-corridor algorithm for structured layouts.
 *
 * This generator creates rectangular rooms connected by corridors, providing more
 * predictable and tactical environments compared to cellular automata generation.
 * The algorithm divides the map into a grid of potential room areas and generates
 * rooms with connecting corridors.
 *
 * @tparam MapSizeX Width of the map in tiles
 * @tparam MapSizeY Height of the map in tiles
 */
template<int64_t MapSizeX, int64_t MapSizeY>
class RoomCorridorGenerator {
public:
  static constexpr int64_t map_size_x = MapSizeX;
  static constexpr int64_t map_size_y = MapSizeY;

  using MapGrid = std::array<std::array<bool, map_size_x>, map_size_y>;

  /**
   * @brief Represents a rectangular room in the map.
   */
  struct Room {
    int x, y, width, height;
  };

  /**
   * @brief Configuration parameters for room and corridor generation.
   */
  struct GenerationParams {
    /// Minimum size for room width/height
    int min_room_size = 4;
    /// Maximum size for room width/height
    int max_room_size = 8;
    /// Number of potential room areas per axis (creates rooms_per_axis^2 potential rooms)
    int rooms_per_axis = 4;
    /// Probability that a room will be generated in each potential area [0.0, 1.0]
    float room_spawn_probability = 0.7f;
    /// Probability of adding interior walls within rooms for tactical complexity [0.0, 1.0]
    float interior_wall_probability = 0.15f;
    /// Random seed for reproducible generation
    uint32_t seed = 0;
  };

  /**
   * @brief Constructs a room and corridor generator with the given parameters.
   *
   * @param params Configuration for generation algorithm
   * @pre params.min_room_size > 0 and params.max_room_size >= params.min_room_size
   * @pre params.rooms_per_axis > 0
   * @pre params.room_spawn_probability and params.interior_wall_probability are in [0.0, 1.0]
   */
  explicit RoomCorridorGenerator(const GenerationParams& params = {});

  /**
   * @brief Generates a new map using the room and corridor algorithm.
   *
   * Creates a grid where true represents walls and false represents walkable areas.
   * The algorithm ensures all rooms are connected via corridors and that border
   * walls are maintained.
   *
   * @return MapGrid with generated layout
   * @post Border tiles (edges) are always walls
   * @post All walkable areas are connected
   */
  MapGrid generate();

  /**
   * @brief Checks if a tile at the given coordinates is a wall.
   *
   * @param x X coordinate to check
   * @param y Y coordinate to check
   * @return true if the tile is a wall, false if walkable
   * @note Coordinates outside map bounds are considered walls
   */
  [[nodiscard]] bool is_wall(int x, int y) const;

  /**
   * @brief Returns the complete generated grid.
   *
   * @return Reference to the current map grid
   * @pre generate() must have been called at least once
   */
  [[nodiscard]] const MapGrid& get_grid() const;

  /**
   * @brief Returns the map width.
   * @return Map width in tiles
   */
  [[nodiscard]] constexpr int64_t get_map_size_x() const { return map_size_x; }

  /**
   * @brief Returns the map height.
   * @return Map height in tiles
   */
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
  std::vector<Room> rooms_;

  /**
   * @brief Initializes the grid with all walls.
   * @param grid Grid to initialize
   */
  void initialize_with_walls(MapGrid& grid);

  /**
   * @brief Generates rooms in a grid pattern across the map.
   * @param grid Grid to modify
   * @post rooms_ vector contains all generated rooms
   */
  void generate_rooms(MapGrid& grid);

  /**
   * @brief Creates corridors connecting all generated rooms.
   * @param grid Grid to modify
   * @pre rooms_ must contain at least one room
   */
  void connect_rooms(MapGrid& grid);

  /**
   * @brief Adds random interior walls within rooms for tactical complexity.
   * @param grid Grid to modify
   */
  void add_interior_walls(MapGrid& grid);

  /**
   * @brief Creates an L-shaped corridor between two rooms.
   * @param grid Grid to modify
   * @param from Source room
   * @param to Destination room
   */
  void create_corridor(MapGrid& grid, const Room& from, const Room& to);

  /**
   * @brief Ensures all border tiles are walls.
   * @param grid Grid to modify
   * @post All edge tiles are set to true (walls)
   */
  void ensure_border_walls(MapGrid& grid);

  /**
   * @brief Checks if two rooms overlap.
   * @param a First room
   * @param b Second room
   * @return true if rooms overlap, false otherwise
   */
  bool rooms_overlap(const Room& a, const Room& b) const;
};

} // namespace wiz

#include "wiz/map/room_corridor_generator.inl"