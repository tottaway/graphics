#include "components/collider.hh"
#include "geometry/rectangle_utils.hh"
#include "utility/try.hh"
#include <algorithm>
#include <iostream>
#include <optional>
#include <ostream>

namespace component {
namespace {
uint16_t get_iteraction_mask_for_interaction_type(
    const InteractionType interaction_type) {
  switch (interaction_type) {
  case InteractionType::unspecified: {
    return unspecified_collider_interaction_mask;
  }
  case InteractionType::hit_box_collider: {
    return hit_box_collider_interaction_mask;
  }
  case InteractionType::hurt_box_collider: {
    return hurt_box_collider_interaction_mask;
  }
  case InteractionType::wiz_good_hit_box_collider: {
    return wiz_good_hit_box_collider_interaction_mask;
  }
  case InteractionType::wiz_neutral_hit_box_collider: {
    return wiz_neutral_hit_box_collider_interaction_mask;
  }
  case InteractionType::wiz_bad_hit_box_collider: {
    return wiz_bad_hit_box_collider_interaction_mask;
  }
  case InteractionType::wiz_good_hurt_box_collider: {
    return wiz_good_hurt_box_collider_interaction_mask;
  }
  case InteractionType::wiz_neutral_hurt_box_collider: {
    return wiz_neutral_hurt_box_collider_interaction_mask;
  }
  case InteractionType::wiz_bad_hurt_box_collider: {
    return wiz_bad_hurt_box_collider_interaction_mask;
  }
  case InteractionType::wiz_grass_tile_collider: {
    return wiz_grass_tile_collider_interaction_mask;
  }
  case InteractionType::solid_collider: {
    return solid_collider_interaction_mask;
  }
  case InteractionType::max_value: {
    std::cout
        << "UNREACHABLE CODE HIT IN COLLIDER: max value interaction type found"
        << std::endl;
    return 0;
  }
  }
  return 0;
}
} // namespace

Collider::Collider(const ColliderType _collider_type, const Shape _shape,
                   GetTransformFunc _get_transform, const MoveFunc move_func)
    : collider_type(_collider_type), shape(_shape),
      get_transform(_get_transform), get_bounds([this]() {
        return geometry::get_bottom_left_and_top_right_from_transform(
            get_transform());
      }),
      collision_callback([](const model::EntityID) {}), move_func_(move_func) {}

Collider::Collider(const ColliderType _collider_type, const Shape _shape,
                   GetTransformFunc _get_transform, const MoveFunc move_func,
                   const CollisionCallback _collision_callback)
    : collider_type(_collider_type), shape(_shape),
      get_transform(_get_transform), get_bounds([this]() {
        return geometry::get_bottom_left_and_top_right_from_transform(
            get_transform());
      }),
      collision_callback(_collision_callback), move_func_(move_func) {}

Collider::Collider(const ColliderType _collider_type, const Shape _shape,
                   GetTransformFunc _get_transform, GetBoundsFunc _get_bounds,
                   const MoveFunc move_func,
                   const CollisionCallback _collision_callback)
    : collider_type(_collider_type), shape(_shape),
      get_transform(_get_transform), get_bounds(_get_bounds),
      collision_callback(_collision_callback), move_func_(move_func) {}

void Collider::update_translation(const Eigen::Vector2f translation) {
  if (maybe_translation_) {
    maybe_translation_.value() += translation;
    return;
  }
  maybe_translation_ = translation;
}

[[nodiscard]] Result<void, std::string> Collider::late_update() {
  maybe_bottom_left_top_right = std::nullopt;
  if (maybe_translation_.has_value()) {
    move_func_(maybe_translation_.value());
  }
  maybe_translation_ = std::nullopt;
  return Ok();
}

bool Collider::bounds_collide(Collider &other) {
  if (!maybe_bottom_left_top_right) {
    maybe_bottom_left_top_right = get_bounds();
  }
  if (!other.maybe_bottom_left_top_right) {
    other.maybe_bottom_left_top_right = other.get_bounds();
  }
  const auto [bottom_left, top_right] = maybe_bottom_left_top_right.value();
  const auto [other_bottom_left, other_top_right] =
      other.maybe_bottom_left_top_right.value();

  return ((other_top_right.x() > bottom_left.x()) &&
          (top_right.x() > other_bottom_left.x()) &&
          (other_top_right.y() > bottom_left.y()) &&
          (top_right.y() > other_bottom_left.y()));
}

void Collider::set_interaction_type(const InteractionType interaction_type) {
  interaction_type_ = static_cast<uint16_t>(interaction_type);
  interaction_mask_ =
      get_iteraction_mask_for_interaction_type(interaction_type);
}

SolidAABBCollider::SolidAABBCollider(GetTransformFunc get_transform,
                                     const MoveFunc move_func)
    : Collider(ColliderType::solid, Shape::aabb, get_transform, move_func) {
  set_interaction_type(InteractionType::solid_collider);
}

bool SolidAABBCollider::handle_collision(Collider &other) {
  const auto &[bottom_left, top_right] = get_bounds();
  const auto &[other_bottom_left, other_top_right] = other.get_bounds();

  switch (other.shape) {
  case Shape::aabb: {
    // TODO have this logic in two places now :sadpanda
    if (other.collider_type == ColliderType::non_collidable) {
      return true;
    }
    std::array<Eigen::Vector2f, 4> moves;
    // move left
    moves[0] = Eigen::Vector2f{bottom_left.x() - other_top_right.x(), 0.f};
    // move right
    moves[1] = Eigen::Vector2f{top_right.x() - other_bottom_left.x(), 0.f};
    // move down
    moves[2] = Eigen::Vector2f{0.f, bottom_left.y() - other_top_right.y()};
    // move up
    moves[3] = Eigen::Vector2f{0.f, top_right.y() - other_bottom_left.y()};

    const auto translation = *std::min_element(
        moves.begin(), moves.end(), [](const auto &first, const auto &second) {
          return first.norm() < second.norm();
        });

    if (other.collider_type == ColliderType::solid) {
      other.update_translation(translation / 2);
      update_translation(-translation / 2);
    } else if (other.collider_type == ColliderType::static_object) {
      std::cout << "UNREACHABLE CODE HIT IN COLLIDER" << std::endl;
    }
    return true;
  }
  default:
    std::cout << "UNREACHABLE CODE HIT IN COLLIDER" << std::endl;
    return true;
  }
}

NonCollidableAABBCollider::NonCollidableAABBCollider(
    GetTransformFunc get_transform, CollisionCallback collision_callback)
    : Collider(
          ColliderType::non_collidable, Shape::aabb, get_transform,
          [](const Eigen::Vector2f) {}, collision_callback) {}

NonCollidableAABBCollider::NonCollidableAABBCollider(
    GetTransformFunc get_transform, GetBoundsFunc get_bounds,
    CollisionCallback collision_callback)
    : Collider(
          ColliderType::non_collidable, Shape::aabb, get_transform, get_bounds,
          [](const Eigen::Vector2f) {}, collision_callback) {}

bool NonCollidableAABBCollider::handle_collision(Collider &other) {
  switch (other.shape) {
  case Shape::aabb: {
    return true;
  }
  default:
    std::cout << "UNREACHABLE CODE HIT IN COLLIDER" << std::endl;
    return true;
  }
}
} // namespace component
