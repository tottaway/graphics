#include "components/grid_collider.hh"

namespace component {
GridCollider::GridCollider(const GetCellsFunc _get_cells,
                           const HandleCollisionFunc _handle_collision)
    : get_cells(_get_cells), handle_collision(_handle_collision) {}

} // namespace component
