#include "wiz/map/tree_tile.hh"
#include "components/sprite.hh"
#include "geometry/rectangle_utils.hh"
#include "view/tileset/texture_set.hh"
#include "wiz/map/grass_tile.hh"
#include <random>

namespace wiz {
TreeTile::TreeTile(model::GameState &game_state) : model::Entity(game_state) {}

Result<void, std::string> TreeTile::init(const Eigen::Vector2f position) {
  position_ = position;
  const auto *texture_set = TRY(view::TextureSet::parse_texture_set(
      std::filesystem::path(texture_set_path)));

  const auto tree_textures = texture_set->get_texture_set_by_name("tree");

  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> texture_index_dist(
      0, tree_textures.size() - 1);
  const auto tree_texture = tree_textures[texture_index_dist(rng)];

  TRY(add_child_entity_and_init<GrassTile>(position_));

  add_component<component::Sprite>([this, tree_texture]() {
    return component::Sprite::SpriteInfo{
        get_transform()
            .translate(Eigen::Vector2f{0.0, 1.5})
            .scale(Eigen::Vector2f{1.5, 2.}),
        tree_texture};
  });
  return Ok();
}

Eigen::Affine2f TreeTile::get_transform() const {
  return geometry::make_rectangle_from_center_and_size(
      position_, Eigen::Vector2f{0.05f, 0.05f});
}

} // namespace wiz
