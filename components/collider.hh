#pragma once
#include "components/component.hh"
#include "model/entity_id.hh"

#include <Eigen/Dense>
#include <bit>
#include <cstddef>
#include <map>
#include <optional>
#include <unordered_set>

namespace component {

enum class ColliderType {
  static_object = 0,
  non_collidable = 1,
  solid = 2,
};

enum class Shape {
  aabb = 0,
};

enum class InteractionType : uint16_t {
  unspecified = 0U,
  hit_box_collider = 1U << 0,
  hurt_box_collider = 1U << 1,
  wiz_good_hurt_box_collider = 1U << 2,
  wiz_bad_hurt_box_collider = 1U << 3,
  wiz_neutral_hurt_box_collider = 1U << 4,
  wiz_good_hit_box_collider = 1U << 5,
  wiz_bad_hit_box_collider = 1U << 6,
  wiz_neutral_hit_box_collider = 1U << 7,
  wiz_grass_tile_collider = 1U << 8,
  solid_collider = 1U << 9,
  jump_reset_collider = 1U << 10,
  jumper_collider = 1U << 11,
  lightmaze_light_volume = 1U << 12,
  lightmaze_platform_collider = 1U << 13,
  max_value = 1U << 14,
};

static constexpr uint16_t unspecified_collider_interaction_mask{
    std::numeric_limits<uint16_t>::max()};

static constexpr uint16_t hit_box_collider_interaction_mask{
    static_cast<uint16_t>(InteractionType::hurt_box_collider)};

static constexpr uint16_t hurt_box_collider_interaction_mask{
    static_cast<uint16_t>(InteractionType::hit_box_collider)};

static constexpr uint16_t wiz_good_hurt_box_collider_interaction_mask{
    static_cast<uint16_t>(InteractionType::wiz_bad_hit_box_collider) |
    static_cast<uint16_t>(InteractionType::wiz_neutral_hit_box_collider)};

static constexpr uint16_t wiz_bad_hurt_box_collider_interaction_mask{
    static_cast<uint16_t>(InteractionType::wiz_good_hit_box_collider) |
    static_cast<uint16_t>(InteractionType::wiz_neutral_hit_box_collider)};

static constexpr uint16_t wiz_neutral_hurt_box_collider_interaction_mask{
    static_cast<uint16_t>(InteractionType::wiz_good_hit_box_collider) |
    static_cast<uint16_t>(InteractionType::wiz_neutral_hit_box_collider) |
    static_cast<uint16_t>(InteractionType::wiz_bad_hit_box_collider)};

static constexpr uint16_t wiz_good_hit_box_collider_interaction_mask{
    static_cast<uint16_t>(InteractionType::wiz_bad_hurt_box_collider) |
    static_cast<uint16_t>(InteractionType::wiz_neutral_hurt_box_collider)};

static constexpr uint16_t wiz_bad_hit_box_collider_interaction_mask{
    static_cast<uint16_t>(InteractionType::wiz_good_hurt_box_collider) |
    static_cast<uint16_t>(InteractionType::wiz_neutral_hurt_box_collider)};

static constexpr uint16_t wiz_neutral_hit_box_collider_interaction_mask{
    static_cast<uint16_t>(InteractionType::wiz_good_hurt_box_collider) |
    static_cast<uint16_t>(InteractionType::wiz_neutral_hurt_box_collider) |
    static_cast<uint16_t>(InteractionType::wiz_bad_hurt_box_collider)};

static constexpr uint16_t wiz_grass_tile_collider_interaction_mask{
    static_cast<uint16_t>(InteractionType::solid_collider)};

static constexpr uint16_t solid_collider_interaction_mask{
    static_cast<uint16_t>(InteractionType::solid_collider) |
    static_cast<uint16_t>(InteractionType::wiz_grass_tile_collider)};

static constexpr uint16_t jump_reset_collider_interaction_mask{
    static_cast<uint16_t>(InteractionType::jumper_collider)};

static constexpr uint16_t jumper_collider_interaction_mask{
    static_cast<uint16_t>(InteractionType::jump_reset_collider)};

static constexpr uint16_t lightmaze_light_volume_interaction_mask{
    static_cast<uint16_t>(InteractionType::lightmaze_platform_collider)};

static constexpr uint16_t lightmaze_platform_collider_interaction_mask{
    static_cast<uint16_t>(InteractionType::lightmaze_light_volume)};

class Collider : public Component {
public:
  static constexpr std::string_view component_type_name = "collider_component";

  using MoveFunc = std::function<void(const Eigen::Vector2f &)>;
  using GetTransformFunc = std::function<Eigen::Affine2f()>;
  using CollisionCallback = std::function<void(const model::EntityID)>;
  using GetBoundsFunc =
      std::function<std::pair<Eigen::Vector2f, Eigen::Vector2f>()>;

  [[nodiscard]] virtual std::string_view get_component_type_name() const {
    return component_type_name;
  }

  [[nodiscard]] virtual std::string_view get_collider_type_name() const = 0;

  [[nodiscard]] virtual Result<void, std::string> late_update();

  virtual bool handle_collision(Collider &other) = 0;

  ColliderType collider_type;
  Shape shape;
  GetTransformFunc get_transform;
  GetBoundsFunc get_bounds;
  CollisionCallback collision_callback;

  void update_translation(const Eigen::Vector2f translation);

  std::optional<std::pair<Eigen::Vector2f, Eigen::Vector2f>>
      maybe_bottom_left_top_right{};

  void set_interaction_type(const InteractionType interaction_type);
  std::size_t get_interaction_type_index() const {
    return std::countr_zero(interaction_type_);
  }

  bool check_collider_types_interact(Collider &other) {
    return ((interaction_mask_ & other.interaction_type_) &&
            (other.interaction_mask_ & interaction_type_)) ||
           (!interaction_type_ && !other.interaction_type_);
  }

protected:
  Collider(const ColliderType _collider_type, const Shape _shape,
           GetTransformFunc _get_transform, const MoveFunc move_func);

  Collider(const ColliderType _collider_type, const Shape _shape,
           GetTransformFunc _get_transform, const MoveFunc move_func,
           const CollisionCallback _collision_callback);

  Collider(const ColliderType _collider_type, const Shape _shape,
           GetTransformFunc _get_transform, GetBoundsFunc _get_bounds,
           const MoveFunc move_func,
           const CollisionCallback _collision_callback);

  bool bounds_collide(Collider &other);

private:
  MoveFunc move_func_;

  std::optional<Eigen::Vector2f> maybe_translation_;

  /// Indicates which classes of colliders this collider can interact with. If
  /// none then we can collide with any colliders
  uint16_t interaction_type_{
      static_cast<uint16_t>(InteractionType::unspecified)};
  uint16_t interaction_mask_{std::numeric_limits<uint16_t>::max()};
};

class SolidAABBCollider : public Collider {
public:
  static constexpr std::string_view collider_type_name = "solid_collider";

  [[nodiscard]] virtual std::string_view get_collider_type_name() const override {
    return collider_type_name;
  }

  SolidAABBCollider(GetTransformFunc get_transform, const MoveFunc move_func);

  virtual bool handle_collision(Collider &other) override;
};

class NonCollidableAABBCollider : public Collider {
public:
  static constexpr std::string_view collider_type_name = "non_collidable_collider";

  [[nodiscard]] virtual std::string_view get_collider_type_name() const override {
    return collider_type_name;
  }

  NonCollidableAABBCollider(GetTransformFunc get_transform,
                            CollisionCallback collision_callback);

  NonCollidableAABBCollider(GetTransformFunc get_transform,
                            GetBoundsFunc get_bounds,
                            CollisionCallback collision_callback);

  virtual bool handle_collision(Collider &other) override;
};

class StaticAABBCollider : public Collider {
public:
  static constexpr std::string_view collider_type_name = "static_collider";

  [[nodiscard]] virtual std::string_view get_collider_type_name() const override {
    return collider_type_name;
  }

  StaticAABBCollider(GetTransformFunc get_transform);

  virtual bool handle_collision(Collider &other) override;
};
} // namespace component
