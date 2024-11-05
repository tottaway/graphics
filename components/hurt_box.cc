#include "components/hurt_box.hh"
#include "components/collider.hh"

namespace component {
HurtBox::HurtBox(GetTransformFunc get_transform, HandleHurtFunc handle_hit)
    : NonCollidableAABBCollider(
          get_transform,
          [handle_hit](const model::EntityID) { handle_hit(); }) {

  maybe_collider_types_to_interact_with_ = {{"hit_box_collider"}};
}
} // namespace component
