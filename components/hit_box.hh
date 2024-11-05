#pragma once

#include "components/collider.hh"
#include "components/component.hh"

namespace component {

class HitBox : public NonCollidableAABBCollider {
public:
  static constexpr std::string_view collider_type_name = "hit_box_collider";

  HitBox(GetTransformFunc get_transform);

  [[nodiscard]] virtual std::optional<std::string_view>
  get_collider_type() const override {
    return collider_type_name;
  }
};

} // namespace component
