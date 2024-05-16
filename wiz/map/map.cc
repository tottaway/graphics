#include "wiz/map/map.hh"
#include "model/game_state.hh"
#include "wiz/map/grass_tile.hh"
#include "wiz/map/tree_tile.hh"
#include <chrono>
#include <random>

namespace wiz {

Map::Map(model::GameState &game_state) : model::Entity(game_state) {}

Result<void, std::string> Map::init() {
  for (const auto i : std::ranges::views::iota(0, 50)) {
    for (const auto j : std::ranges::views::iota(0, 50)) {
      std::random_device dev;
      std::mt19937 rng(dev());
      std::uniform_int_distribution<std::mt19937::result_type> dist(
          1, 100); // distribution in range [1, 6]
      if (dist(rng) > 98) {
        TRY(add_child_entity_and_init<TreeTile>(Eigen::Vector2f{
            static_cast<float>(i) * 0.1f, static_cast<float>(j) * 0.1f}));
      } else {
        TRY(add_child_entity_and_init<GrassTile>(Eigen::Vector2f{
            static_cast<float>(i) * 0.1f, static_cast<float>(j) * 0.1f}));
      }
    }
  }
  return Ok();
}
} // namespace wiz
