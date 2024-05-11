#include "components/draw_grid_cell.hh"
#include "components/draw_rectangle.hh"

namespace component {

DrawGridCell::DrawGridCell(const Eigen::Vector2i grid_size,
                           const float cell_size, GetCellInfoFunc get_info)
    : DrawRectangle([get_info, grid_size, cell_size]() {
        const auto cell_info = get_info();
        // divide by two because rectangle have a max side length of 2
        const auto cell_draw_size = cell_size / 2;
        return RectangleInfo{
Eigen::Affine2f(Eigen::Translation2f{
      cell_info.transform.cast<float>() * cell_size}).scale(cell_draw_size),
            cell_info.color};
      }) {}

} // namespace component
