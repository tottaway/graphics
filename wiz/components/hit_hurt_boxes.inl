#pragma once

#include <string_view>
namespace wiz {
namespace {

static constexpr std::string_view good_hurt_box_name{
    "wiz_good_hurt_box_collider"};
static constexpr std::string_view good_hit_box_name{
    "wiz_good_hit_box_collider"};
static constexpr std::string_view bad_hurt_box_name{
    "wiz_bad_hurt_box_collider"};
static constexpr std::string_view bad_hit_box_name{"wiz_bad_hit_box_collider"};

} // namespace
template <Alignement alignement>
WizHitBox<alignement>::WizHitBox(GetTransformFunc get_transform)
    : HitBox(get_transform) {
  std::string_view collider_type_to_interact_with;
  if constexpr (alignement == Alignement::good) {
    collider_type_to_interact_with = bad_hurt_box_name;
  } else {
    collider_type_to_interact_with = good_hurt_box_name;
  }

  if (maybe_collider_types_to_interact_with_.has_value()) {
    maybe_collider_types_to_interact_with_.value().emplace_back(
        collider_type_to_interact_with);
  } else {
    maybe_collider_types_to_interact_with_ = {{collider_type_to_interact_with}};
  }
}

template <Alignement alignement>
std::optional<std::string_view>
WizHitBox<alignement>::get_collider_type() const {
  if constexpr (alignement == Alignement::good) {
    return good_hit_box_name;
  } else {
    return bad_hit_box_name;
  }
}

template <Alignement alignement>
WizHurtBox<alignement>::WizHurtBox(GetTransformFunc get_transform,
                                   HandleHurtFunc handle_hit)
    : HurtBox(get_transform, handle_hit) {
  std::string_view collider_type_to_interact_with;
  if constexpr (alignement == Alignement::good) {
    collider_type_to_interact_with = bad_hit_box_name;
  } else {
    collider_type_to_interact_with = good_hit_box_name;
  }

  if (maybe_collider_types_to_interact_with_.has_value()) {
    maybe_collider_types_to_interact_with_.value().emplace_back(
        collider_type_to_interact_with);
  } else {
    maybe_collider_types_to_interact_with_ = {{collider_type_to_interact_with}};
  }
}

template <Alignement alignement>
std::optional<std::string_view>
WizHurtBox<alignement>::get_collider_type() const {
  if constexpr (alignement == Alignement::good) {
    return good_hurt_box_name;
  } else {
    return bad_hurt_box_name;
  }
}

} // namespace wiz
