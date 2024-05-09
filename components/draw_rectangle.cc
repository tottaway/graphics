#include "components/draw_rectangle.hh"
#include "geometry/rectangle_utils.hh"
#include "view/screen.hh"

namespace component {

DrawRectangle::DrawRectangle(const model::GameState &game_state,
                             const model::EntityID entity_id,
                             const view::Color &color)
    : game_state_(game_state), entity_id_(entity_id), color_(color) {}

[[nodiscard]] Result<void, std::string>
DrawRectangle::draw(view::Screen &screen) const {
  const auto maybe_entity =
      game_state_.try_get_entity_pointer_by_id(entity_id_);

  if (!maybe_entity) {
    return Err(
        std::string("DrawRectangle's entity component no longer exists"));
  }
  const auto transform = maybe_entity.value()->get_transform();
  const auto [bottom_left, top_right] =
      geometry::get_bottom_left_and_top_right_from_transform(transform);
  screen.draw_rectangle(bottom_left, top_right, color_);
  return Ok();
}
} // namespace component
