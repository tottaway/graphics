#pragma once

#include "components/collider.hh"
#include "components/component.hh"

namespace component {

class HurtBox : public NonCollidableAABBCollider {
public:
  using HandleHurtFunc = std::function<void()>;
  HurtBox(GetTransformFunc get_transform, HandleHurtFunc handle_hit);
};

} // namespace component
