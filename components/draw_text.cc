#include "components/draw_text.hh"
#include "view/screen.hh"

namespace component {

DrawText::DrawText(const model::GameState &game_state,
                   const model::EntityID entity_id, const float font_size,
                   std::string text)
    : game_state_(game_state), entity_id_(entity_id), font_size_(font_size),
      text_(std::move(text)) {}

[[nodiscard]] Result<void, std::string>
DrawText::draw(view::Screen &screen) const {
  const auto maybe_entity =
      game_state_.try_get_entity_pointer_by_id(entity_id_);

  if (!maybe_entity) {
    return Err(std::string("DrawText's entity component no longer exists"));
  }
  const auto translation = maybe_entity.value()->get_transform().translation();
  screen.draw_text(translation, font_size_, text_);
  return Ok();
}
} // namespace component
