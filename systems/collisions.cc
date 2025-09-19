#include "systems/collisions.hh"
#include "components/collider.hh"
#include "geometry/rectangle_utils.hh"
#include "model/entity_id.hh"
#include <Eigen/Dense>
#include <iostream>
#include <ranges>

namespace systems {

struct Bounds {
  float x_max;
  float x_min;
  float y_max;
  float y_min;
};

template <std::size_t x_dim, std::size_t y_dim> class QuadTree {
public:
  void add_collider(component::Collider *collider,
                    const model::EntityID entity_id);

private:
  static constexpr Bounds max_bounds{7.0f, -1.0f, 7.0f, -1.0f};
  static constexpr float cell_size_x{(max_bounds.x_max - max_bounds.x_min) /
                                     static_cast<float>(x_dim)};
  static constexpr float cell_size_y{(max_bounds.y_max - max_bounds.y_min) /
                                     static_cast<float>(y_dim)};

  struct QuadTreeElement {
    component::Collider *collider;
    model::EntityID entity_id;
    Bounds bounds;
  };

  struct QuadTreeNode {
    std::unordered_map<std::size_t, std::vector<QuadTreeElement>> elements;
  };

  std::array<QuadTreeNode, x_dim * y_dim> nodes_{QuadTreeNode{{}}};
};

bool bounds_overlap(const Bounds &a, const Bounds &b) {
  return a.x_max > b.x_min && b.x_max > a.x_min && a.y_max > b.y_min &&
         b.y_max > a.y_min;
}

template <std::size_t x_dim, std::size_t y_dim>
void QuadTree<x_dim, y_dim>::add_collider(component::Collider *collider,
                                          const model::EntityID entity_id) {
  const auto [bottom_left, top_right] = collider->get_bounds();
  const Bounds entity_bounds = {top_right.x(), bottom_left.x(), top_right.y(),
                                bottom_left.y()};

  const auto min_x = std::max(
      0UL, static_cast<size_t>((entity_bounds.x_min - max_bounds.x_min) /
                               cell_size_x));
  const auto max_x =
      std::min(x_dim - 1, static_cast<size_t>((entity_bounds.x_max +
                                               cell_size_x - max_bounds.x_min) /
                                              cell_size_x));
  const auto min_y = std::max(
      0UL, static_cast<size_t>((entity_bounds.y_min - max_bounds.y_min) /
                               cell_size_y));
  const auto max_y =
      std::min(y_dim - 1, static_cast<size_t>((entity_bounds.y_max +
                                               cell_size_y - max_bounds.y_min) /
                                              cell_size_y));

  for (const auto i : std::ranges::views::iota(min_x, max_x)) {
    for (const auto j : std::ranges::views::iota(min_y, max_y)) {
      auto &node = nodes_[i + j * x_dim];
      for (const auto &[_, interaction_type_specific_elements] :
           node.elements) {
        if (!interaction_type_specific_elements.empty() &&
            !collider->check_collider_types_interact(
                *interaction_type_specific_elements.begin()->collider)) {
          continue;
        }
        for (const auto &element : interaction_type_specific_elements) {
          if (bounds_overlap(element.bounds, entity_bounds)) {
            if (collider->handle_collision(*element.collider)) {
              collider->collision_callback(element.entity_id);
              element.collider->collision_callback(entity_id);
            }
          }
        }
      }
      node.elements[collider->get_interaction_type_index()].emplace_back(
          collider, entity_id, entity_bounds);
    }
  }
}

Collisions::Collisions(model::GameState &game_state)
    : game_state_(game_state) {}

Result<void, std::string> Collisions::update(const int64_t delta_time_ns) {
  QuadTree<10, 10> quad_tree;

  const auto entities =
      game_state_.get_entities_with_component<component::Collider>();
  for (const auto i : std::ranges::views::iota(size_t{0}, entities.size())) {
    auto entity = entities.at(i);
    auto colliders = entity->get_components<component::Collider>();
    for (auto collider : colliders) {
      quad_tree.add_collider(collider, entity->get_entity_id());
    }
  }
  return Ok();
}
} // namespace systems
