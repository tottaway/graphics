#include "wiz/map/map.hh"
#include "model/game_state.hh"
#include "wiz/map/grass_tile.hh"
#include "wiz/map/tree_tile.hh"
#include <random>

namespace wiz {

Map::Map(model::GameState &game_state) : model::Entity(game_state) {}

Result<void, std::string> Map::init() {
  for (const auto i : std::ranges::views::iota(0, 30)) {
    for (const auto j : std::ranges::views::iota(0, 30)) {
      std::random_device dev;
      std::mt19937 rng(dev());
      std::uniform_int_distribution<std::mt19937::result_type> dist(1, 100);
      if (dist(rng) > 98) {
        const auto entity = TRY(add_child_entity_and_init<TreeTile>(
            Eigen::Vector2f{static_cast<float>(i) * tile_size,
                            static_cast<float>(j) * tile_size}));
        map_tiles_[i][j] = entity->get_entity_id();
      } else {
        const auto entity = TRY(add_child_entity_and_init<GrassTile>(
            Eigen::Vector2f{static_cast<float>(i) * tile_size,
                            static_cast<float>(j) * tile_size}));
        map_tiles_[i][j] = entity->get_entity_id();
      }
    }
  }
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

[[nodiscard]] Result<model::EntityID, std::string>
Map::get_map_tile_entity_by_index(const Eigen::Vector2i tile_index) const {
  if (tile_index.x() < 0 || tile_index.x() >= map_size_x ||
      tile_index.y() < 0 || tile_index.y() >= map_size_y) {
    return Err(std::string("Tile index out of bounds"));
  }

  return Ok(map_tiles_[tile_index.x()][tile_index.y()]);
}
} // namespace wiz
