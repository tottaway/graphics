#include "components/jump_reset.hh"
#include "components/collider.hh"

namespace component {

JumpReset::JumpReset(GetTransformFunc get_transform)
    : NonCollidableAABBCollider(get_transform, [](const model::EntityID) {
        // No callback needed - the Jumper component handles the interaction
      }) {
  set_interaction_type(InteractionType::jump_reset_collider);
}

} // namespace component