#include "model/rectangle.hh"
#include "components/draw_rectangle.hh"
#include "view/screen.hh"

namespace model {
StaticDrawnRectangle::StaticDrawnRectangle(model::GameState &game_state)
    : Entity(game_state) {}

void StaticDrawnRectangle::init(const Eigen::Affine2f &static_transform,
                                const view::Color &color) {
  static_transform_ = static_transform;
  components_.emplace_back(std::make_unique<component::DrawRectangle>(
      game_state_, get_entity_id(), color));
}

} // namespace model
