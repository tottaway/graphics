#include "wiz/enemies/skeleton_spawner.hh"
#include "components/collider.hh"
#include "components/sprite.hh"
#include "geometry/rectangle_utils.hh"
#include "view/tileset/texture_set.hh"
#include "wiz/enemies/skeleton.hh"
#include <chrono>
#include <iostream>
#include <random>

namespace wiz {
SkeletonSpawner::SkeletonSpawner(model::GameState &game_state)
    : model::Entity(game_state) {}

Result<void, std::string>
SkeletonSpawner::init(const Eigen::Vector2f position) {

  // add_component<component::SolidAABBCollider>(
  //     [this]() { return get_transform(); },
  //     [this](const Eigen::Vector2f translation) mutable {
  //       position_ += translation;
  //     });

  position_ = position;
  const auto *texture_set = TRY(view::TextureSet::parse_texture_set(
      std::filesystem::path(texture_set_path)));

  const auto texture = texture_set->get_texture_set_by_name("big_stone")[0];

  add_component<component::Sprite>([this, texture]() {
    return component::Sprite::SpriteInfo{get_transform(), texture};
  });
  return Ok();
}

Eigen::Affine2f SkeletonSpawner::get_transform() const {
  return geometry::make_rectangle_from_center_and_size(
      position_, Eigen::Vector2f{0.07f, 0.1f});
}

[[nodiscard]] Result<void, std::string>
SkeletonSpawner::update(const int64_t delta_time_ns) {
  if (duration_since_last_spawn_ns_ > spawn_interval_ns) {
    std::ignore = TRY(add_child_entity_and_init<Skeleton>(position_));
    duration_since_last_spawn_ns_ = 0L;
  }
  duration_since_last_spawn_ns_ += delta_time_ns;
  return Ok();
}

} // namespace wiz
