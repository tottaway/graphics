#pragma once
#include "components/component.hh"
#include "model/entity_id.hh"
#include "model/game_state.hh"
#include "view/screen.hh"

namespace component {
class DrawRectangle : public Component {
public:
  DrawRectangle(const model::GameState &game_state,
                const model::EntityID entity_id, const view::Color &color);

  [[nodiscard]] virtual Result<void, std::string>
  draw(view::Screen &screen) const final;

private:
  const model::GameState &game_state_;
  model::EntityID entity_id_;
  view::Color color_;
};
} // namespace component
