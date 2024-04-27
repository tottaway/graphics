#pragma once
#include "model/game_state.hh"
#include "view/screen.hh"
#include <Eigen/Dense>

namespace model {

// TODO this should probably be a UiElement as opposed to an entity
class Rectangle : public Entity {
public:
  static constexpr std::string_view entity_type_name = "rectangle";
  virtual ~Rectangle() = default;
  Rectangle(model::GameState &game_state);

  void init(const view::Box &box, const view::Color &color);

  /// Draw the entity on the screen
  [[nodiscard]] virtual Result<void, std::string>
  draw(view::Screen &screen) const;

  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  };

private:
  view::Box box_;
  view::Color color_;
};
} // namespace model
