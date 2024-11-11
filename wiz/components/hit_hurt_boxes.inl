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
static constexpr std::string_view neutral_hurt_box_name{
    "wiz_neutral_hurt_box_collider"};
static constexpr std::string_view neutral_hit_box_name{
    "wiz_neutral_hit_box_collider"};

} // namespace
template <Alignement alignement>
WizHitBox<alignement>::WizHitBox(GetTransformFunc get_transform)
    : HitBox(get_transform) {
  if (!maybe_collider_types_to_interact_with_.has_value()) {
    maybe_collider_types_to_interact_with_.emplace();
  }
  auto &collider_type_to_interact_with =
      maybe_collider_types_to_interact_with_.value();
  if constexpr (alignement == Alignement::good) {
    collider_type_to_interact_with.push_back(bad_hurt_box_name);
    collider_type_to_interact_with.push_back(neutral_hurt_box_name);
  } else if constexpr (alignement == Alignement::neutral) {
    collider_type_to_interact_with.push_back(good_hurt_box_name);
    collider_type_to_interact_with.push_back(bad_hurt_box_name);
    collider_type_to_interact_with.push_back(neutral_hurt_box_name);
  } else if constexpr (alignement == Alignement::bad) {
    collider_type_to_interact_with.push_back(good_hurt_box_name);
    collider_type_to_interact_with.push_back(neutral_hurt_box_name);
  } else {
    static_assert("Unhandled alignement in WizHitBox");
  }
}

template <Alignement alignement>
std::optional<std::string_view>
WizHitBox<alignement>::get_collider_type() const {
  if constexpr (alignement == Alignement::good) {
    return good_hit_box_name;
  } else if constexpr (alignement == Alignement::neutral) {
    return neutral_hit_box_name;
  } else {
    return bad_hit_box_name;
  }
}

template <Alignement alignement>
WizHurtBox<alignement>::WizHurtBox(GetTransformFunc get_transform,
                                   HandleHurtFunc handle_hit)
    : HurtBox(get_transform, handle_hit) {
  if (!maybe_collider_types_to_interact_with_.has_value()) {
    maybe_collider_types_to_interact_with_.emplace();
  }
  auto &collider_type_to_interact_with =
      maybe_collider_types_to_interact_with_.value();
  if constexpr (alignement == Alignement::good) {
    collider_type_to_interact_with.push_back(bad_hit_box_name);
    collider_type_to_interact_with.push_back(neutral_hit_box_name);
  } else if constexpr (alignement == Alignement::neutral) {
    collider_type_to_interact_with.push_back(good_hit_box_name);
    collider_type_to_interact_with.push_back(bad_hit_box_name);
    collider_type_to_interact_with.push_back(neutral_hit_box_name);
  } else if constexpr (alignement == Alignement::bad) {
    collider_type_to_interact_with.push_back(good_hit_box_name);
    collider_type_to_interact_with.push_back(neutral_hit_box_name);
  } else {
    static_assert("Unhandled alignement in WizHitBox");
  }
}

template <Alignement alignement>
std::optional<std::string_view>
WizHurtBox<alignement>::get_collider_type() const {
  if constexpr (alignement == Alignement::good) {
    return good_hurt_box_name;
  } else if constexpr (alignement == Alignement::neutral) {
    return neutral_hurt_box_name;
  } else {
    return bad_hurt_box_name;
  }
}

} // namespace wiz
