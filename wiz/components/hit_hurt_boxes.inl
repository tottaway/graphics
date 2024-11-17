#pragma once

#include <string_view>
namespace wiz {
template <Alignement alignement>
WizHitBox<alignement>::WizHitBox(GetTransformFunc get_transform)
    : HitBox(get_transform) {
  if constexpr (alignement == Alignement::good) {
    set_interaction_type(component::InteractionType::wiz_good_hit_box_collider);
  } else if constexpr (alignement == Alignement::neutral) {
    set_interaction_type(
        component::InteractionType::wiz_neutral_hit_box_collider);
  } else if constexpr (alignement == Alignement::bad) {
    set_interaction_type(component::InteractionType::wiz_bad_hit_box_collider);
  } else {
    static_assert("Unhandled alignement in WizHitBox");
  }
}

template <Alignement alignement>
WizHurtBox<alignement>::WizHurtBox(GetTransformFunc get_transform,
                                   HandleHurtFunc handle_hit)
    : HurtBox(get_transform, handle_hit) {
  if constexpr (alignement == Alignement::good) {
    set_interaction_type(
        component::InteractionType::wiz_good_hurt_box_collider);
  } else if constexpr (alignement == Alignement::neutral) {
    set_interaction_type(
        component::InteractionType::wiz_neutral_hurt_box_collider);
  } else if constexpr (alignement == Alignement::bad) {
    set_interaction_type(component::InteractionType::wiz_bad_hurt_box_collider);
  } else {
    static_assert("Unhandled alignement in WizHitBox");
  }
}
} // namespace wiz
