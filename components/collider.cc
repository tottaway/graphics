#include "components/collider.hh"
#include "geometry/rectangle_utils.hh"
#include "utility/try.hh"
#include <algorithm>
#include <optional>

namespace component {
Collider::Collider(const ColliderType _collider_type, const Shape _shape,
                   GetTransformFunc _get_transform, const MoveFunc move_func)
    : collider_type(_collider_type), shape(_shape),
      get_transform(_get_transform),
      collision_callback([](const model::EntityID) {}), move_func_(move_func) {}

Collider::Collider(const ColliderType _collider_type, const Shape _shape,
                   GetTransformFunc _get_transform, const MoveFunc move_func,
                   const CollisionCallback _collision_callback)
    : collider_type(_collider_type), shape(_shape),
      get_transform(_get_transform), collision_callback(_collision_callback),
      move_func_(move_func) {}

void Collider::update_translation(const Eigen::Vector2f translation) {
  if (maybe_translation_) {
    maybe_translation_.value() += translation;
    return;
  }
  maybe_translation_ = translation;
}

[[nodiscard]] Result<void, std::string> Collider::late_update() {
  if (maybe_translation_.has_value()) {
    move_func_(maybe_translation_.value());
  }
  maybe_translation_ = std::nullopt;
  return Ok();
}

bool Collider::bounds_collide(Collider &other) {
  const auto &[bottom_left, top_right] =
      geometry::get_bottom_left_and_top_right_from_transform(get_transform());
  const auto &[other_bottom_left, other_top_right] =
      geometry::get_bottom_left_and_top_right_from_transform(
          other.get_transform());

  return !((other_top_right.x() < bottom_left.x()) ||
           (top_right.x() < other_bottom_left.x()) ||
           (other_top_right.y() < bottom_left.y()) ||
           (top_right.y() < other_bottom_left.y()));
}

SolidAABBCollider::SolidAABBCollider(GetTransformFunc get_transform,
                                     const MoveFunc move_func)
    : Collider(ColliderType::solid, Shape::aabb, get_transform, move_func) {}

bool SolidAABBCollider::handle_collision(Collider &other) {
  if (!bounds_collide(other)) {
    return false;
  }

  const auto &[bottom_left, top_right] =
      geometry::get_bottom_left_and_top_right_from_transform(get_transform());
  const auto &[other_bottom_left, other_top_right] =
      geometry::get_bottom_left_and_top_right_from_transform(
          other.get_transform());

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

bool NonCollidableAABBCollider::handle_collision(Collider &other) {
  if (!bounds_collide(other)) {
    return false;
  }
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
