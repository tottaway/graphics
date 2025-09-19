#pragma once
#include "model/entity_id.hh"
#include "model/game_state.hh"
#include "wiz/map/cellular_automata_generator.hh"
#include "wiz/map/room_corridor_generator.hh"

namespace wiz {

/**
 * @brief Map generation algorithm options.
 */
enum class MapGenerationAlgorithm {
  CellularAutomata,
  RoomCorridor
};

/**
 * @brief Defines different types of movement interactions with map tiles.
 *
 * This enum allows different entities to have different movement rules
 * on the same tile types, providing flexibility for gameplay mechanics.
 */
enum class MapInteractionType {
  /// Can walk on both grass tiles and tiles with flowers
  walk_on_grass_and_flowers,
  /// Can only walk on grass tiles without flowers
  walk_only_on_grass,
  /// Can only walk on grass tiles that have flowers
  walk_only_on_flowers
};

/**
 * @brief Main map entity that manages the game world grid and all tile entities.
 *
 * The Map class is responsible for:
 * - Creating and managing a grid of tile entities (GrassTile, WallTile, etc.)
 * - Providing coordinate conversion between world positions and tile indices
 * - Offering tile lookup and validation methods for gameplay systems
 * - Serving as the parent entity for all map tiles
 *
 * The map internally decides which type of tile to place at each grid position
 * based on its generation logic, rather than accepting external tile type arrays.
 * All tiles are created as child entities during initialization.
 *
 * @note Tile entities are owned by this Map entity and will be automatically
 *       cleaned up when the Map is destroyed.
 */
class Map : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "wiz_map";
  static constexpr MapGenerationAlgorithm generation_algorithm = MapGenerationAlgorithm::CellularAutomata;

  /**
   * @brief Construct a new Map entity.
   * @param game_state Reference to the game state that will own this map
   */
  Map(model::GameState &game_state);

  /**
   * @brief Initialize the map by creating all tile entities in the grid.
   *
   * This method creates a grid of map tiles as child entities. The specific
   * type of tile created at each position is determined by the map's internal
   * generation logic (currently all grass, but will be enhanced with walls).
   *
   * @return Ok() on success, Err(message) if tile creation fails
   */
  Result<void, std::string> init();

  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  };

  /**
   * @brief Get the tile entity at a given world position.
   * @param position World coordinates to query
   * @return EntityID of the tile at that position, or error if out of bounds
   */
  [[nodiscard]] Result<model::EntityID, std::string>
  get_map_tile_entity_by_position(const Eigen::Vector2f position) const;

  /**
   * @brief Convert world position to tile grid coordinates.
   * @param position World coordinates to convert
   * @return Grid indices (x, y) for the tile containing that position
   */
  [[nodiscard]] Eigen::Vector2i
  get_tile_index_by_position(const Eigen::Vector2f position) const;

  /**
   * @brief Convert tile grid coordinates to world position.
   * @param tile_index Grid coordinates to convert
   * @return World position of the center of that tile
   */
  [[nodiscard]] Eigen::Vector2f
  get_tile_position_by_index(const Eigen::Vector2i tile_index) const;

  /**
   * @brief Get the tile entity at specific grid coordinates.
   * @param tile_index Grid coordinates to query
   * @return EntityID of the tile at those coordinates, or error if out of bounds
   */
  [[nodiscard]] Result<model::EntityID, std::string>
  get_map_tile_entity_by_index(const Eigen::Vector2i tile_index) const;

  /**
   * @brief Check if a tile allows movement based on interaction type.
   * @param tile_index Grid coordinates to check
   * @param interaction_type The type of movement interaction to check
   * @return true if the tile allows the specified movement, false otherwise
   */
  [[nodiscard]] bool is_walkable_tile(const Eigen::Vector2i tile_index,
                                      MapInteractionType interaction_type) const;

  /**
   * @brief Check if grid coordinates are within map bounds.
   * @param tile_index Grid coordinates to validate
   * @return true if coordinates are valid, false if out of bounds
   */
  [[nodiscard]] bool is_valid_tile_index(const Eigen::Vector2i tile_index) const;

  /**
   * @brief Check if a grass tile has flowers (legacy function).
   * @param tile_index Grid coordinates to check
   * @return true if the tile is grass and has flowers, false otherwise
   * @deprecated Use is_walkable_tile() with appropriate MapInteractionType instead
   */
  [[nodiscard]] bool
  get_map_tile_is_grass_and_has_flowers(const Eigen::Vector2i tile_index) const;

private:
  static constexpr int64_t map_size_x{30UL};
  static constexpr int64_t map_size_y{30UL};
  static constexpr float tile_size{0.2f};

  std::array<std::array<model::EntityID, map_size_x>, map_size_y> map_tiles_;
};
} // namespace wiz
