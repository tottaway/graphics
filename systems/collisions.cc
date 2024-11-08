#include "systems/collisions.hh"
#include "components/collider.hh"
#include "geometry/rectangle_utils.hh"
#include "model/entity_id.hh"
#include <Eigen/Dense>
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
  static constexpr Bounds max_bounds{5, -5, 5, -5};
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
    std::vector<QuadTreeElement> elements;
  };

  void exand_node(QuadTreeNode &node);
  std::array<QuadTreeNode, x_dim * y_dim> nodes_{QuadTreeNode{{}}};
};

bool bounds_overlap(const Bounds &a, const Bounds &b) {
  return a.x_max > b.x_min && b.x_max > a.x_min && a.y_max > b.y_min &&
         b.y_max > a.y_min;
}

template <std::size_t x_dim, std::size_t y_dim>
void QuadTree<x_dim, y_dim>::add_collider(component::Collider *collider,
                                          const model::EntityID entity_id) {
  const auto [bottom_left, top_right] =
      geometry::get_bottom_left_and_top_right_from_transform(
          collider->get_transform());
  const Bounds entity_bounds = {top_right.x(), bottom_left.x(), top_right.y(),
                                bottom_left.y()};

  for (const auto i : std::ranges::views::iota(0UL, x_dim)) {
    for (const auto j : std::ranges::views::iota(0UL, y_dim)) {
      auto &node = nodes_[i + j * x_dim];
      Bounds node_bounds = {max_bounds.x_min + (i + 1) * cell_size_x,
                            max_bounds.x_min + i * cell_size_x,
                            max_bounds.y_min + (j + 1) * cell_size_y,
                            max_bounds.y_min + j * cell_size_y};
      if (bounds_overlap(entity_bounds, node_bounds)) {
        for (const auto &element : node.elements) {
          if (bounds_overlap(element.bounds, entity_bounds)) {
            if (collider->handle_collision(*element.collider)) {
              collider->collision_callback(element.entity_id);
              element.collider->collision_callback(entity_id);
            }
          }
        }
        node.elements.emplace_back(collider, entity_id, entity_bounds);
      }
    }
  }
}

Collisions::Collisions(model::GameState &game_state)
    : game_state_(game_state) {}

Result<void, std::string> Collisions::update(const int64_t delta_time_ns) {
  QuadTree<10, 10> quad_tree;

  const auto entities =
      game_state_.get_entities_with_component<component::Collider>();
  for (const auto i : std::ranges::views::iota(0UL, entities.size())) {
    auto entity = entities.at(i);
    auto colliders = entity->get_components<component::Collider>();
    for (auto collider : colliders) {
      quad_tree.add_collider(collider, entity->get_entity_id());
    }
  }
  return Ok();
}
} // namespace systems
