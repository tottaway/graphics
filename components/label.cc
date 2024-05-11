#include "components/label.hh"
#include "geometry/rectangle_utils.hh"
#include "view/screen.hh"

namespace component {

Label::Label(GetTextInfoFunc get_text_info) : get_text_info_(get_text_info) {}

[[nodiscard]] Result<void, std::string>
Label::draw(view::Screen &screen) const {
  const auto text_info = get_text_info_();

  screen.draw_text(text_info.transform.translation(), text_info.font,
                   text_info.text, text_info.color);
  return Ok();
}
} // namespace component
