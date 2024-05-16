#include "wiz/map/grass_tile.hh"
#include "components/sprite.hh"
#include "geometry/rectangle_utils.hh"
#include "view/tileset/texture_set.hh"
#include <chrono>
#include <iostream>
#include <random>

namespace wiz {
GrassTile::GrassTile(model::GameState &game_state)
    : model::Entity(game_state) {}

Result<void, std::string> GrassTile::init(const Eigen::Vector2f position) {

  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist(
      1, 10); // distribution in range [1, 6]
  std::string texture_set_name = dist(rng) > 8 ? "flower" : "grass";

  position_ = position;
  const auto *texture_set = TRY(view::TextureSet::parse_texture_set(
      std::filesystem::path(texture_set_path)));

  const auto textures = texture_set->get_texture_set_by_name(texture_set_name);

  std::uniform_int_distribution<std::mt19937::result_type> texture_index_dist(
      0, textures.size() - 1);
  const auto texture = textures[texture_index_dist(rng)];

  transform_ = geometry::make_rectangle_from_center_and_size(
      position_, Eigen::Vector2f{0.1f, 0.1f});

  const auto sprite_info = component::Sprite::SpriteInfo{transform_, texture};
  add_component<component::Sprite>([sprite_info]() { return sprite_info; });
  return Ok();
}

Eigen::Affine2f GrassTile::get_transform() const { return transform_; }

} // namespace wiz
