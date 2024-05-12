#include "components/draw_grid_cell.hh"
#include "components/draw_rectangle.hh"
#include "geometry/transform_utils.hh"

namespace component {

DrawGridCell::DrawGridCell(const Eigen::Vector2f &cell_size,
                           GetCellInfoFunc get_info)
    : DrawRectangle([get_info, cell_size]() {
        const auto cell_info = get_info();
        return RectangleInfo{
            geometry::transform_from_grid_cell(cell_info.transform, cell_size),
            cell_info.color};
      }) {}

} // namespace component
