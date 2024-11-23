#pragma once

#include "components/component.hh"
#include "components/hit_box.hh"
#include "components/hurt_box.hh"

namespace wiz {

enum class Alignement {
  good = 0,
  bad = 1,
  neutral = 2,
};

template <Alignement alignement> class WizHitBox : public component::HitBox {
public:
  WizHitBox(GetTransformFunc get_transform);
};

template <Alignement alignement> class WizHurtBox : public component::HurtBox {
public:
  WizHurtBox(GetTransformFunc get_transform, HandleHurtFunc handle_hit);
  WizHurtBox(GetTransformFunc get_transform, GetBoundsFunc _get_bounds,
             HandleHurtFunc handle_hit);
};

} // namespace wiz

#include "wiz/components/hit_hurt_boxes.inl"
