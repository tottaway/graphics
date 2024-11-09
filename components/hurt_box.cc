#include "components/hurt_box.hh"
#include "components/collider.hh"

namespace component {
HurtBox::HurtBox(GetTransformFunc get_transform, HandleHurtFunc handle_hit)
    : NonCollidableAABBCollider(
          get_transform,
          [handle_hit](const model::EntityID) { handle_hit(); }) {

  if (maybe_collider_types_to_interact_with_.has_value()) {
    maybe_collider_types_to_interact_with_.value().emplace_back(
        "hit_box_collider");
  } else {
    maybe_collider_types_to_interact_with_ = {{"hit_box_collider"}};
  }
}
} // namespace component
