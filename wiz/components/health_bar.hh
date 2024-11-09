#pragma once
#include "components/draw_rectangle.hh"

namespace wiz {
class HealthBar : public component::DrawRectangle {
public:
  static constexpr std::string_view component_type_name =
      "health_bar_component";

  using GetHitPointsFunc = std::function<int32_t()>;
  using GetActorTransformFunc = std::function<Eigen::Affine2f()>;
  HealthBar(const int32_t max_hp, GetHitPointsFunc get_hp,
            GetActorTransformFunc get_actor_transform_func);

  [[nodiscard]] virtual std::string_view get_component_type_name() const {
    return component_type_name;
  }

private:
  int32_t max_hp_;
  GetHitPointsFunc get_hp_;
  GetActorTransformFunc get_actor_transform_func_;
};
} // namespace wiz
