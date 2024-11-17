#include "components/hit_box.hh"
#include "components/collider.hh"

namespace component {
HitBox::HitBox(GetTransformFunc get_transform)
    : NonCollidableAABBCollider(get_transform, [](const model::EntityID) {}) {
  set_interaction_type(InteractionType::hit_box_collider);
}
} // namespace component
