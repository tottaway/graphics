#include "components/center.hh"
#include "view/screen.hh"

namespace component {

Center::Center(GetTransformFunc get_transform)
    : get_transform_(get_transform) {}

[[nodiscard]] Result<void, std::string>
Center::draw(view::Screen &screen) const {
  const auto transform = get_transform_();
  screen.set_viewport_center(transform.translation());
  return Ok();
}
} // namespace component
