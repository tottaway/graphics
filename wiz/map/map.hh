#pragma once
#include "model/entity_id.hh"
#include "model/game_state.hh"

namespace wiz {
class Map : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "wiz_map";
  Map(model::GameState &game_state);

  Result<void, std::string> init();

  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  };

  [[nodiscard]] Result<model::EntityID, std::string>
  get_map_tile_entity_by_position(const Eigen::Vector2f position) const;

  [[nodiscard]] Eigen::Vector2i
  get_tile_index_by_position(const Eigen::Vector2f position) const;

  [[nodiscard]] Eigen::Vector2f
  get_tile_position_by_index(const Eigen::Vector2i tile_index) const;

  [[nodiscard]] Result<model::EntityID, std::string>
  get_map_tile_entity_by_index(const Eigen::Vector2i tile_index) const;

  [[nodiscard]] bool
  get_map_tile_is_grass_and_has_flowers(const Eigen::Vector2i tile_index) const;

private:
  static constexpr int64_t map_size_x{30UL};
  static constexpr int64_t map_size_y{30UL};
  static constexpr float tile_size{0.1f};

  std::array<std::array<model::EntityID, map_size_x>, map_size_y> map_tiles_;
};
} // namespace wiz
