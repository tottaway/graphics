#include "wiz/movable_stone.hh"
#include "components/collider.hh"
#include "components/sprite.hh"
#include "geometry/rectangle_utils.hh"
#include "view/tileset/texture_set.hh"
#include <chrono>
#include <iostream>
#include <random>

namespace wiz {
MovableStone::MovableStone(model::GameState &game_state)
    : model::Entity(game_state) {}

Result<void, std::string> MovableStone::init(const Eigen::Vector2f position) {

  add_component<component::SolidAABBCollider>(
      [this]() { return get_transform(); },
      [this](const Eigen::Vector2f translation) mutable {
        position_ += translation;
        transform_ = geometry::make_rectangle_from_center_and_size(
            position_, Eigen::Vector2f{0.05f, 0.05f});
      });

  position_ = position;
  const auto *texture_set = TRY(view::TextureSet::parse_texture_set(
      std::filesystem::path(texture_set_path)));

  const auto texture = texture_set->get_texture_set_by_name("big_stone")[0];

  transform_ = geometry::make_rectangle_from_center_and_size(
      position_, Eigen::Vector2f{0.05f, 0.05f});

  add_component<component::Sprite>([this, texture]() {
    return component::Sprite::SpriteInfo{transform_, texture};
  });
  return Ok();
}

Eigen::Affine2f MovableStone::get_transform() const { return transform_; }

} // namespace wiz
