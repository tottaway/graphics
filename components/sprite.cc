#include "components/sprite.hh"
#include "geometry/rectangle_utils.hh"
#include "view/screen.hh"

namespace component {

Sprite::Sprite(GetSpriteInfoFunc get_info) : get_info_(get_info) {}

[[nodiscard]] Result<void, std::string>
Sprite::draw(view::Screen &screen) const {
  const auto info = get_info_();
  const auto [bottom_left, top_right] =
      geometry::get_bottom_left_and_top_right_from_transform(info.transform);
  screen.draw_rectangle(bottom_left, top_right, info.texture);
  return Ok();
}
} // namespace component
