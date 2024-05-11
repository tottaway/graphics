#include "components/draw_rectangle.hh"
#include "geometry/rectangle_utils.hh"
#include "view/screen.hh"

namespace component {

DrawRectangle::DrawRectangle(GetRectangleInfoFunc get_info)
    : get_info_(get_info) {}

[[nodiscard]] Result<void, std::string>
DrawRectangle::draw(view::Screen &screen) const {
  const auto info = get_info_();
  const auto [bottom_left, top_right] =
      geometry::get_bottom_left_and_top_right_from_transform(info.transform);
  screen.draw_rectangle(bottom_left, top_right, info.color);
  return Ok();
}
} // namespace component
