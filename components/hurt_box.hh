#pragma once

#include "components/collider.hh"
#include "components/component.hh"

namespace component {

class HurtBox : public NonCollidableAABBCollider {
public:
  static constexpr std::string_view collider_type_name = "hurt_box_collider";

  using HandleHurtFunc = std::function<void()>;
  HurtBox(GetTransformFunc get_transform, HandleHurtFunc handle_hit);

  [[nodiscard]] virtual std::optional<std::string_view>
  get_collider_type() const override {
    return collider_type_name;
  }
};

} // namespace component
