#include "model/rectangle.hh"
#include "view/screen.hh"

namespace model {
Rectangle::Rectangle(model::GameState &game_state) : Entity(game_state) {}

void Rectangle::init(const view::Box &box, const view::Color &color) {
  box_ = box;
  color_ = color;
}

Result<void, std::string> Rectangle::draw(view::Screen &screen) const {
  screen.draw_rectangle(box_, color_);
  return Ok();
}
} // namespace model
