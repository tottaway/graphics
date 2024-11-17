#include "components/hurt_box.hh"
#include "components/collider.hh"

namespace component {
HurtBox::HurtBox(GetTransformFunc get_transform, HandleHurtFunc handle_hit)
    : NonCollidableAABBCollider(
          get_transform,
          [handle_hit](const model::EntityID) { handle_hit(); }) {
  set_interaction_type(InteractionType::hurt_box_collider);
}
} // namespace component
