#pragma once
#include "model/game_state.hh"
#include "wiz/character_mode.hh"

namespace wiz {
class Worker : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "wiz_worker";

  Worker(model::GameState &game_state);

  Result<void, std::string> init(const Eigen::Vector2f position);

  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  };

  [[nodiscard]] virtual Result<void, std::string>
  update(const int64_t delta_time_ns);

  [[nodiscard]] virtual Eigen::Affine2f get_transform() const;

  [[nodiscard]] virtual bool should_remove() override { return off_flowers_; }

private:
  static constexpr std::string_view worker_texture_set_path{
      "sprites/wiz/workers/sprites.yaml"};

  enum class WorkerColor { cyan, purple, lime, red };

  Result<void, std::string> update_direction();

  CharacterMode mode_{CharacterMode::idle};
  Eigen::Vector2f position_{0.f, 0.f};
  Eigen::Vector2f direction_{0.1, 0.1};
  bool off_flowers_{false};
};
} // namespace wiz
