#pragma once
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
};
} // namespace wiz
