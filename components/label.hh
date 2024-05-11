#pragma once
#include "components/component.hh"
#include "model/entity_id.hh"
#include "model/game_state.hh"
#include "view/screen.hh"

namespace component {
class Label : public Component {
public:
  static constexpr std::string_view component_type_name =
      "draw_rectangle_component";

  struct TextInfo {
    std::string_view text;
    view::Color color;
    float font;
    Eigen::Affine2f transform;
  };

  using GetTextInfoFunc = std::function<TextInfo()>;
  Label(GetTextInfoFunc get_text_info);

  [[nodiscard]] virtual Result<void, std::string>
  draw(view::Screen &screen) const final;

  [[nodiscard]] virtual std::string_view get_component_type_name() const {
    return component_type_name;
  }

private:
  GetTextInfoFunc get_text_info_;
};
} // namespace component
