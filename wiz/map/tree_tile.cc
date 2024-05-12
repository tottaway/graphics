#include "wiz/map/tree_tile.hh"
#include "components/sprite.hh"
#include "geometry/rectangle_utils.hh"

namespace wiz {
TreeTile::TreeTile(model::GameState &game_state) : model::Entity(game_state) {}

Result<void, std::string> TreeTile::init(const Eigen::Vector2f position) {
  position_ = position;
  view::Texture tree_texture{std::filesystem::path(tree_texture_path),
                             Eigen::Vector2i{0, 0}, Eigen::Vector2i{64, 90}};
  view::Texture grass_texture{std::filesystem::path(ground_texture_path),
                              Eigen::Vector2i{0, 0}, Eigen::Vector2i{48, 48}};

  add_component<component::Sprite>([this, grass_texture]() {
    return component::Sprite::SpriteInfo{get_transform(), grass_texture};
  });
  add_component<component::Sprite>([this, tree_texture]() {
    return component::Sprite::SpriteInfo{
        get_transform()
            .translate(Eigen::Vector2f{0., 70.f / 64.f})
            .scale(Eigen::Vector2f{1., 90.f / 64.f}),
        tree_texture};
  });
  return Ok();
}

Eigen::Affine2f TreeTile::get_transform() const {
  return geometry::make_rectangle_from_center_and_size(
      position_, Eigen::Vector2f{0.2f, 0.2f});
}

} // namespace wiz
