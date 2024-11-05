#pragma once
#include "components/component.hh"
#include "model/entity_id.hh"

#include <Eigen/Dense>
#include <cstddef>
#include <map>
#include <optional>

namespace component {

enum class ColliderType {
  static_object = 0,
  non_collidable = 1,
  solid = 2,
};

enum class Shape {
  aabb = 0,
};

class Collider : public Component {
public:
  static constexpr std::string_view component_type_name = "collider_component";

  using MoveFunc = std::function<void(const Eigen::Vector2f &)>;
  using GetTransformFunc = std::function<Eigen::Affine2f()>;
  using CollisionCallback = std::function<void(const model::EntityID)>;

  [[nodiscard]] virtual std::string_view get_component_type_name() const {
    return component_type_name;
  }

  /// Allow specifify a specific collider type to allow colliders to define
  /// interactions between other types (i.e. hit boxes and hurt boxes)
  [[nodiscard]] virtual std::optional<std::string_view>
  get_collider_type() const {
    return std::nullopt;
  }

  [[nodiscard]] virtual Result<void, std::string> late_update();

  virtual bool handle_collision(Collider &other) = 0;

  ColliderType collider_type;
  Shape shape;
  GetTransformFunc get_transform;
  CollisionCallback collision_callback;

  void update_translation(const Eigen::Vector2f translation);

protected:
  Collider(const ColliderType _collider_type, const Shape _shape,
           GetTransformFunc _get_transform, const MoveFunc move_func);

  Collider(const ColliderType _collider_type, const Shape _shape,
           GetTransformFunc _get_transform, const MoveFunc move_func,
           const CollisionCallback _collision_callback);

  bool bounds_collide(Collider &other);

  bool check_collider_types_interact(Collider &other);

  /// optional list of collider types this collider can collide with, if it's
  /// none then we can collide with any colliders
  std::optional<std::vector<std::string_view>>
      maybe_collider_types_to_interact_with_;

private:
  MoveFunc move_func_;

  std::optional<Eigen::Vector2f> maybe_translation_;
};

class SolidAABBCollider : public Collider {
public:
  SolidAABBCollider(GetTransformFunc get_transform, const MoveFunc move_func);

  virtual bool handle_collision(Collider &other) override;
};

class NonCollidableAABBCollider : public Collider {
public:
  NonCollidableAABBCollider(GetTransformFunc get_transform,
                            CollisionCallback collision_callback);

  virtual bool handle_collision(Collider &other) override;
};
} // namespace component
