#pragma once
#include "model/game_state.hh"

namespace wiz {
class TreeTile : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "wiz_tree_tile";
  TreeTile(model::GameState &game_state);

  Result<void, std::string> init(const Eigen::Vector2f position);

  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  };

  [[nodiscard]] virtual Eigen::Affine2f get_transform() const;

  [[nodiscard]] virtual uint8_t get_z_level() const { return 2; }

private:
  static constexpr std::string_view texture_set_path{
      "sprites/wiz/map_textures/texture_set.yaml"};
  Eigen::Vector2f position_;
};
} // namespace wiz
