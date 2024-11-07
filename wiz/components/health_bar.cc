#include "wiz/components/health_bar.hh"
#include "components/draw_rectangle.hh"
#include "geometry/rectangle_utils.hh"

namespace wiz {
HealthBar::HealthBar(GetHitPointsFunc get_hp,
                     GetActorTransformFunc get_actor_transform_func)
    : DrawRectangle([this]() {
        const auto [bottom_left, top_right] =
            geometry::get_bottom_left_and_top_right_from_transform(
                get_actor_transform_func_());
        const auto center = bottom_left + ((top_right - bottom_left) / 2.f);
        auto identity = Eigen::Affine2f::Identity();
        return component::DrawRectangle::RectangleInfo{
            identity.translate(center)
                .translate(Eigen::Vector2f{0.f, 0.17f})
                .scale(Eigen::Vector2f{0.08f, 0.02f}),
            view::Color{255, 0, 0}};
      }),
      get_hp_(get_hp), get_actor_transform_func_(get_actor_transform_func) {}
} // namespace wiz
