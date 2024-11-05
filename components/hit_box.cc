#include "components/hit_box.hh"
#include "components/collider.hh"

namespace component {
HitBox::HitBox(GetTransformFunc get_transform)
    : NonCollidableAABBCollider(get_transform, [](const model::EntityID) {}) {
  maybe_collider_types_to_interact_with_ = {{"hurt_box_collider"}};
}
} // namespace component
