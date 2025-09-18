#include "wiz/map/wall_tile.hh"
#include "components/collider.hh"
#include "components/sprite.hh"
#include "geometry/rectangle_utils.hh"
#include "view/tileset/texture_set.hh"
#include <random>

namespace wiz {

WallTile::WallTile(model::GameState &game_state) : model::Entity(game_state) {}

Result<void, std::string> WallTile::init(const Eigen::Vector2f position,
                                         const float size) {
  position_ = position;
  // Shrink wall tiles by epsilon to prevent touching adjacent walls from
  // triggering warnings
  constexpr float epsilon = 1e-7f;
  const Eigen::Vector2f tile_size_vec{size * 0.5f - epsilon,
                                      size * 0.5f - epsilon};
  transform_ =
      geometry::make_rectangle_from_center_and_size(position_, tile_size_vec);

  // Add static collision - walls are immovable obstacles
  add_component<component::StaticAABBCollider>(
      [this]() { return get_transform(); });

  // Load stone texture
  const auto *texture_set = TRY(view::TextureSet::parse_texture_set(
      std::filesystem::path(texture_set_path)));

  const auto stone_texture_set =
      texture_set->get_texture_set_by_name(stone_texture_name);

  // Use random stone texture variant
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> stone_dist(
      0, stone_texture_set.size() - 1);
  maybe_stone_texture_.emplace(stone_texture_set[stone_dist(rng)]);

  // Add sprite component for rendering
  add_component<component::Sprite>([this]() {
    return component::Sprite::SpriteInfo{transform_,
                                         maybe_stone_texture_.value()};
  });

  return Ok();
}

Eigen::Affine2f WallTile::get_transform() const { return transform_; }

} // namespace wiz
