#pragma once

#include "components/component.hh"
#include "components/hit_box.hh"

namespace wiz {

enum class Alignement {
  good = 0,
  bad = 1,
};

template <Alignement alignement> class WizHitBox : public component::HitBox {
public:
  WizHitBox(GetTransformFunc get_transform);

  [[nodiscard]] virtual std::optional<std::string_view>
  get_collider_type() const override;
};

template <Alignement alignement> class WizHurtBox : public component::HurtBox {
public:
  WizHurtBox(GetTransformFunc get_transform, HandleHurtFunc handle_hit);

  [[nodiscard]] virtual std::optional<std::string_view>
  get_collider_type() const override;
};

} // namespace wiz

#include "wiz/components/hit_hurt_boxes.inl"
