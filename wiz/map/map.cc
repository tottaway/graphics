#include "wiz/map/map.hh"
#include "model/game_state.hh"
// #include "wiz/map/grass_tile.hh"
#include "wiz/map/tree_tile.hh"

namespace wiz {

Map::Map(model::GameState &game_state) : model::Entity(game_state) {}

Result<void, std::string> Map::init() {
  TRY(add_child_entity_and_init<TreeTile>(Eigen::Vector2f{1, 1}));
  return Ok();
}
} // namespace wiz
