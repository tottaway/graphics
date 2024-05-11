#pragma once
#include "components/component.hh"
#include "model/entity_id.hh"
#include "model/game_state.hh"
#include "view/screen.hh"

namespace component {
class DrawRectangle : public Component {
public:
  static constexpr std::string_view component_type_name =
      "draw_rectangle_component";
  struct RectangleInfo {
    Eigen::Affine2f transform;
    view::Color color;
  };

  using GetRectangleInfoFunc = std::function<RectangleInfo()>;
  DrawRectangle(GetRectangleInfoFunc get_info);

  [[nodiscard]] virtual Result<void, std::string>
  draw(view::Screen &screen) const final;

  [[nodiscard]] virtual std::string_view get_component_type_name() const {
    return component_type_name;
  }

private:
  GetRectangleInfoFunc get_info_;
};
} // namespace component
