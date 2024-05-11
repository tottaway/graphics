#pragma once
#include "components/draw_rectangle.hh"
#include "view/screen.hh"

#include <Eigen/Dense>
#include <string_view>

namespace component {
class DrawGridCell : public DrawRectangle {
public:
  static constexpr std::string_view component_type_name =
      "draw_grid_cell_component";
  struct CellInfo {
    Eigen::Vector2i transform;
    view::Color color;
  };

  using GetCellInfoFunc = std::function<CellInfo()>;
  DrawGridCell(const Eigen::Vector2i grid_size, const float cell_size,
               GetCellInfoFunc get_info);

  [[nodiscard]] virtual std::string_view get_component_type_name() const {
    return component_type_name;
  }

private:
  GetRectangleInfoFunc get_info_;
};
} // namespace component
