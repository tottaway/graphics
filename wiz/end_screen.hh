#pragma once

#include "model/game_state.hh"
#include <Eigen/Dense>

namespace wiz {
struct GameResult {
  int64_t survival_duration_ns;
};

class EndScreen : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "wiz_end_screen";
  virtual ~EndScreen() = default;
  EndScreen(model::GameState &game_state);

  void init(const GameResult &game_result);

  [[nodiscard]] virtual Result<bool, std::string>
  on_mouse_up(const view::MouseUpEvent &mouse_up) final;

  [[nodiscard]] virtual Eigen::Affine2f get_transform() const final;

  [[nodiscard]] virtual std::string_view get_entity_type_name() const final {
    return entity_type_name;
  };

  bool has_been_clicked{false};

private:
  static constexpr float font_size = 64.f;
  static constexpr view::Color text_color{94, 108, 87};

  std::string display_text_;
  GameResult result_;
};
} // namespace wiz
