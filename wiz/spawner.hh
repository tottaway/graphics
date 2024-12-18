#pragma once
#include "model/game_state.hh"

namespace wiz {
template <typename SpawnedEnitityType> class Spawner : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "wiz_spawner";
  Spawner(model::GameState &game_state);

  Result<void, std::string> init(const Eigen::Vector2f position);

  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  };

  [[nodiscard]] virtual Result<void, std::string>
  update(const int64_t delta_time_ns);

  [[nodiscard]] virtual Eigen::Affine2f get_transform() const;

  [[nodiscard]] virtual uint8_t get_z_level() const { return 1; }

private:
  static constexpr int64_t spawn_interval_ns{4'000'000'000L};

  Eigen::Vector2f position_{0.5f, 1.f};
  int64_t duration_since_last_spawn_ns_{spawn_interval_ns};
};
} // namespace wiz
#include "wiz/spawner.inl"
