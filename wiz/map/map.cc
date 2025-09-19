#include "wiz/map/map.hh"
#include "model/game_state.hh"
#include "wiz/map/grass_tile.hh"
#include "wiz/map/wall_tile.hh"
#include <iostream>
#include <random>

namespace wiz {

Map::Map(model::GameState &game_state) : model::Entity(game_state) {}

Result<void, std::string> Map::init() {
  std::array<std::array<bool, map_size_x>, map_size_y> grid;

  if constexpr (generation_algorithm == MapGenerationAlgorithm::CellularAutomata) {
    // Generate map using cellular automata
    CellularAutomataGenerator<map_size_x, map_size_y>::GenerationParams ca_params;
    ca_params.initial_wall_probability = 0.3f;
    ca_params.smoothing_iterations = 3;
    ca_params.min_passage_width = 2;
    ca_params.seed = 42;

    CellularAutomataGenerator<map_size_x, map_size_y> ca_generator(ca_params);
    grid = ca_generator.generate();
    ca_generator.print_map_ascii();
  } else {
    // Generate map using room-corridor algorithm
    RoomCorridorGenerator<map_size_x, map_size_y>::GenerationParams rc_params;
    rc_params.min_room_size = 6;
    rc_params.max_room_size = 12;
    rc_params.rooms_per_axis = 3;
    rc_params.room_spawn_probability = 0.8f;
    rc_params.interior_wall_probability = 0.05f;
    rc_params.seed = 42;

    RoomCorridorGenerator<map_size_x, map_size_y> rc_generator(rc_params);
    grid = rc_generator.generate();
    rc_generator.print_map_ascii();
  }

  int wall_count = 0;
  int grass_count = 0;

  for (const auto i : std::ranges::views::iota(int64_t{0}, map_size_x)) {
    for (const auto j : std::ranges::views::iota(int64_t{0}, map_size_y)) {
      const Eigen::Vector2f position{static_cast<float>(i) * tile_size,
                                     static_cast<float>(j) * tile_size};

      if (grid[i][j]) {
        const auto entity = add_child_entity_and_init<WallTile>(position, tile_size);
        if (entity.isErr()) {
          return Err(std::string("Failed to create WallTile at position (") +
                     std::to_string(i) + ", " + std::to_string(j) + "): " + entity.unwrapErr());
        }
        map_tiles_[i][j] = entity.unwrap()->get_entity_id();
        wall_count++;
      } else {
        const auto entity = add_child_entity_and_init<GrassTile>(position, tile_size);
        if (entity.isErr()) {
          return Err(std::string("Failed to create GrassTile at position (") +
                     std::to_string(i) + ", " + std::to_string(j) + "): " + entity.unwrapErr());
        }
        map_tiles_[i][j] = entity.unwrap()->get_entity_id();
        grass_count++;
      }
    }
  }

  // Debug output for map generation
  std::cout << "Map generation complete: " << wall_count << " walls, "
            << grass_count << " grass tiles" << std::endl;

  return Ok();
}

Result<model::EntityID, std::string>
Map::get_map_tile_entity_by_position(const Eigen::Vector2f position) const {
  const auto tile_index = get_tile_index_by_position(position);
  return get_map_tile_entity_by_index(tile_index);
}

[[nodiscard]] Eigen::Vector2i
Map::get_tile_index_by_position(const Eigen::Vector2f position) const {
  return Eigen::Vector2i{
      static_cast<int>((position.x() + tile_size / 2) / tile_size),
      static_cast<int>((position.y() + tile_size / 2) / tile_size)};
}

[[nodiscard]] Eigen::Vector2f
Map::get_tile_position_by_index(const Eigen::Vector2i tile_index) const {
  return Eigen::Vector2f{static_cast<float>(tile_index.x()) * tile_size,
                         static_cast<float>(tile_index.y()) * tile_size};
}

[[nodiscard]] Result<model::EntityID, std::string>
Map::get_map_tile_entity_by_index(const Eigen::Vector2i tile_index) const {
  if (tile_index.x() < 0 || tile_index.x() >= map_size_x ||
      tile_index.y() < 0 || tile_index.y() >= map_size_y) {
    return Err(std::string("Tile index out of bounds"));
  }

  return Ok(map_tiles_[tile_index.x()][tile_index.y()]);
}

bool Map::is_walkable_tile(const Eigen::Vector2i tile_index,
                           MapInteractionType interaction_type) const {
  const auto maybe_tile_entity = get_map_tile_entity_by_index(tile_index);
  if (maybe_tile_entity.isErr()) {
    return false;
  }

  // Check if it's a wall tile (not walkable)
  const auto maybe_wall_tile =
      game_state_.get_entity_pointer_by_id_as<WallTile>(
          maybe_tile_entity.unwrap());
  if (maybe_wall_tile.isOk()) {
    // Wall tiles are never walkable
    return false;
  }

  // Check if it's a grass tile
  const auto maybe_grass_tile =
      game_state_.get_entity_pointer_by_id_as<GrassTile>(
          maybe_tile_entity.unwrap());
  if (maybe_grass_tile.isErr()) {
    // Not a grass tile or wall tile, check for other walkable tile types
    // For now, only grass tiles are walkable
    // TODO: Add checks for other walkable tile types (like paths) here
    return false;
  }

  const auto* grass_tile = maybe_grass_tile.unwrap();

  switch (interaction_type) {
    case MapInteractionType::walk_on_grass_and_flowers:
      // Can walk on grass regardless of flower state
      return true;
    case MapInteractionType::walk_only_on_grass:
      // Can only walk on grass without flowers
      return !grass_tile->has_flowers();
    case MapInteractionType::walk_only_on_flowers:
      // Can only walk on grass with flowers
      return grass_tile->has_flowers();
    default:
      return false;
  }
}

bool Map::is_valid_tile_index(const Eigen::Vector2i tile_index) const {
  return tile_index.x() >= 0 && tile_index.x() < map_size_x &&
         tile_index.y() >= 0 && tile_index.y() < map_size_y;
}

bool Map::get_map_tile_is_grass_and_has_flowers(
    const Eigen::Vector2i tile_index) const {
  const auto maybe_tile_entity = get_map_tile_entity_by_index(tile_index);
  if (maybe_tile_entity.isErr()) {
    // off the map so skip
    return false;
  }

  // not a grass tile so return false
  const auto maybe_grass_tile =
      game_state_.get_entity_pointer_by_id_as<GrassTile>(
          maybe_tile_entity.unwrap());
  if (maybe_grass_tile.isErr()) {
    return false;
  };

  return maybe_grass_tile.unwrap()->has_flowers();
}
} // namespace wiz
