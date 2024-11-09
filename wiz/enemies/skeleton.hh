#pragma once
#include "model/game_state.hh"
#include "wiz/character_mode.hh"
#include <cstdint>

namespace wiz {
class Skeleton : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "wiz_skeleton";
  Skeleton(model::GameState &game_state);

  Result<void, std::string> init(const Eigen::Vector2f position);

  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  };

  [[nodiscard]] virtual Result<void, std::string>
  update(const int64_t delta_time_ns);

  [[nodiscard]] virtual Eigen::Affine2f get_transform() const;

  [[nodiscard]] virtual uint8_t get_z_level() const { return 1; }

  [[nodiscard]] virtual bool should_remove() {
    return mode_ == CharacterMode::dead;
  }

private:
  static constexpr std::string_view skeleton_texture_set_path{
      "sprites/wiz/skeleton/sprites.yaml"};

  static constexpr int64_t max_duration_in_being_hit_ns_{200'000'000L};
  static constexpr int64_t max_duration_in_dying_ns_{200'000'000L};

  int64_t duration_in_being_hit_ns_{0L};
  int64_t duration_dying_hit_ns_{0L};

  bool was_hit_{false};
  int32_t hp_{3};
  CharacterMode mode_{CharacterMode::idle};
  Eigen::Vector2f position_{0.f, 0.f};
  Eigen::Vector2f direction_{0.5f, 0.f};
};
} // namespace wiz
