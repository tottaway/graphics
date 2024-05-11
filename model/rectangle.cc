#include "model/rectangle.hh"
#include "components/draw_rectangle.hh"
#include "view/screen.hh"

namespace model {
StaticDrawnRectangle::StaticDrawnRectangle(model::GameState &game_state)
    : Entity(game_state) {}

void StaticDrawnRectangle::init(const Eigen::Affine2f &static_transform,
                                const view::Color &color) {
  static_transform_ = static_transform;
  add_component<component::DrawRectangle>([this, color]() {
    return component::DrawRectangle::RectangleInfo{get_transform(), color};
  });
}

} // namespace model
