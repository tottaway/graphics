

#include "wiz/end_screen.hh"
#include "components/center.hh"
#include "components/draw_rectangle.hh"
#include "components/label.hh"
#include "model/game_state.hh"
namespace wiz {
EndScreen::EndScreen(model::GameState &game_state)
    : model::Entity(game_state) {}

void EndScreen::init(const GameResult &game_result) {
  result_ = game_result;

  add_component<component::Center>([this]() { return get_transform(); });

  display_text_ =
      std::format("You survived {:.2f} seconds, click to try again",
                  static_cast<double>(game_result.survival_duration_ns) / 1e9);
  add_component<component::Label>([this]() {
    return component::Label::TextInfo{
        display_text_, text_color, font_size,
        get_transform().translate(Eigen::Vector2f{-0.65f, 0.f})};
  });
}

Eigen::Affine2f EndScreen::get_transform() const {
  return Eigen::Affine2f::Identity();
}

Result<bool, std::string>
EndScreen::on_mouse_up(const view::MouseUpEvent &mouse_up) {
  has_been_clicked = true;
  return Ok(false);
}

} // namespace wiz
