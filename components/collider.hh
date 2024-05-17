#pragma once
#include "components/component.hh"
#include "model/entity_id.hh"

#include <Eigen/Dense>
#include <cstddef>

namespace component {

class Collider : public Component {
public:
  static constexpr std::string_view component_type_name = "collider_component";

  using HandleCollisionFunc = std::function<void(const model::EntityID)>;
  Collider(const HandleCollisionFunc _handle_collision);

  [[nodiscard]] virtual std::string_view get_component_type_name() const {
    return component_type_name;
  }

  HandleCollisionFunc handle_collision;
};
} // namespace component
