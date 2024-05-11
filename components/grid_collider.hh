#pragma once
#include "components/component.hh"
#include "model/entity_id.hh"

#include <Eigen/Dense>

namespace component {
class GridCollider : public Component {
public:
  static constexpr std::string_view component_type_name =
      "grid_collider_component";

  using GetCellsFunc = std::function<std::vector<Eigen::Vector2i>()>;
  using HandleCollisionFunc = std::function<void(const model::EntityID)>;
  GridCollider(const GetCellsFunc get_cells,
               const HandleCollisionFunc handle_collision);

  [[nodiscard]] virtual std::string_view get_component_type_name() const {
    return component_type_name;
  }

  GetCellsFunc get_cells;

  HandleCollisionFunc handle_collision;
};
} // namespace component
