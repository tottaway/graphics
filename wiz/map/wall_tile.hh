#pragma once
#include "model/game_state.hh"

namespace wiz {

/**
 * @brief Wall tile entity that provides solid collision barriers.
 *
 * WallTile entities are non-walkable obstacles that block movement
 * and provide solid collision. They are used to create map boundaries
 * and interior obstacles for interesting level layouts.
 */
class WallTile : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "wiz_wall_tile";

  WallTile(model::GameState &game_state);

  /**
   * @brief Initialize the wall tile at the specified position.
   * @param position World coordinates for the center of this wall tile
   * @param size Size of the tile in world units
   * @return Ok() on success, Err(message) if initialization fails
   */
  Result<void, std::string> init(const Eigen::Vector2f position, const float size);

  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  };

  [[nodiscard]] virtual Eigen::Affine2f get_transform() const;

  /// Wall tiles are drawn above grass but below characters
  [[nodiscard]] virtual uint8_t get_z_level() const { return 1; }

private:
  static constexpr std::string_view texture_set_path{
      "sprites/wiz/map_textures/texture_set.yaml"};
  static constexpr std::string_view stone_texture_name{"stone"};

  std::optional<view::Texture> maybe_stone_texture_;
  Eigen::Vector2f position_;
  Eigen::Affine2f transform_;
};

} // namespace wiz