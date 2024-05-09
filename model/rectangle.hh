#pragma once
#include "model/game_state.hh"
#include "view/screen.hh"
#include <Eigen/Dense>

namespace model {

class StaticDrawnRectangle : public Entity {
public:
  static constexpr std::string_view entity_type_name = "static_drawn_rectangle";
  virtual ~StaticDrawnRectangle() = default;
  StaticDrawnRectangle(model::GameState &game_state);

  void init(const Eigen::Affine2f &static_transform, const view::Color &color);

  Eigen::Affine2f get_transform() const final { return static_transform_; }

  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  };

private:
  Eigen::Affine2f static_transform_;
};
} // namespace model
