#include "components/collider.hh"

namespace component {
Collider::Collider(const HandleCollisionFunc _handle_collision)
    : handle_collision(_handle_collision) {}

} // namespace component
