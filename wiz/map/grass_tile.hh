#pragma once
#include "model/game_state.hh"

namespace wiz {
class GrassTile : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "wiz_grass_tile";
  GrassTile(model::GameState &game_state);

  Result<void, std::string> init(const Eigen::Vector2f position, const float size);

  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  };

  [[nodiscard]] virtual Eigen::Affine2f get_transform() const;

  [[nodiscard]] Result<void, std::string> update(const int64_t delta_time_ns);

  [[nodiscard]] bool has_flowers() const { return has_flowers_; }

  void set_has_player() { has_player_ = true; }

private:
  static constexpr std::string_view texture_set_path{
      "sprites/wiz/map_textures/texture_set.yaml"};

  static constexpr std::string_view grass_texture_name{"grass"};
  static constexpr std::string_view flower_texture_name{"flower"};
  static constexpr std::string_view tree_texture_name{"tree"};

  std::optional<view::Texture> maybe_flower_texture_;
  std::optional<view::Texture> maybe_grass_texture_;
  std::optional<view::Texture> maybe_tree_texture_;
  Eigen::Vector2f position_;
  Eigen::Affine2f transform_;
  bool has_tree_{false};
  bool has_flowers_{false};
  bool has_player_{false};
  bool was_hit_{false};
};

class Tree : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "wiz_tree";
  Tree(model::GameState &game_state);

  Result<void, std::string> init(const Eigen::Vector2f position,
                                 view::Texture tree_texture);

  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  };

  [[nodiscard]] virtual Eigen::Affine2f get_transform() const;

  [[nodiscard]] virtual uint8_t get_z_level() const { return 2; }

private:
  Eigen::Affine2f transform_;
};
} // namespace wiz
