#pragma once

#include "components/collider.hh"
#include "components/component.hh"

namespace component {

class HitBox : public NonCollidableAABBCollider {
public:
  HitBox(GetTransformFunc get_transform);
};

} // namespace component
