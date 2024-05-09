#pragma once
#include "components/component.hh"
#include "model/entity_id.hh"
#include "model/game_state.hh"
#include "view/screen.hh"

namespace component {
class DrawText : public Component {
public:
  DrawText(const model::GameState &game_state, const model::EntityID entity_id,
           const float font_size, std::string text);

  [[nodiscard]] virtual Result<void, std::string>
  draw(view::Screen &screen) const final;

private:
  const model::GameState &game_state_;
  model::EntityID entity_id_;
  float font_size_;
  std::string text_;
};
} // namespace component
