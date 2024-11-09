#pragma once

#include "components/collider.hh"
#include "components/sprite.hh"
#include "geometry/rectangle_utils.hh"
#include "view/tileset/texture_set.hh"
#include "wiz/enemies/skeleton.hh"
#include <chrono>
#include <iostream>
#include <random>

namespace wiz {
template <typename SpawnedEnitityType>
Spawner<SpawnedEnitityType>::Spawner(model::GameState &game_state)
    : model::Entity(game_state) {}

template <typename SpawnedEnitityType>
Result<void, std::string>
Spawner<SpawnedEnitityType>::init(const Eigen::Vector2f position) {

  position_ = position;
  return Ok();
}

template <typename SpawnedEnitityType>
Eigen::Affine2f Spawner<SpawnedEnitityType>::get_transform() const {
  return geometry::make_rectangle_from_center_and_size(
      position_, Eigen::Vector2f{0.07f, 0.1f});
}

template <typename SpawnedEnitityType>
[[nodiscard]] Result<void, std::string>
Spawner<SpawnedEnitityType>::update(const int64_t delta_time_ns) {
  if (duration_since_last_spawn_ns_ > spawn_interval_ns) {
    const auto res = add_child_entity_and_init<SpawnedEnitityType>(position_);
    if (res.isErr()) {
      return Err(res.unwrapErr());
    }
    duration_since_last_spawn_ns_ = 0L;
  }
  duration_since_last_spawn_ns_ += delta_time_ns;
  return Ok();
}

} // namespace wiz
