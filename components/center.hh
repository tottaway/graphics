#pragma once
#include "components/component.hh"
#include "view/screen.hh"
#include <Eigen/Dense>

namespace component {
class Center : public Component {
public:
  static constexpr std::string_view component_type_name = "center_component";
  using GetTransformFunc = std::function<Eigen::Affine2f()>;
  Center(GetTransformFunc get_transform);

  [[nodiscard]] virtual Result<void, std::string>
  draw(view::Screen &screen) const final;

  [[nodiscard]] virtual std::string_view get_component_type_name() const {
    return component_type_name;
  }

private:
  GetTransformFunc get_transform_;
};
} // namespace component
