#pragma once
#include "model/game_state.hh"
#include "wiz/character_mode.hh"
#include "wiz/map/map.hh"
#include <deque>
#include <random>

namespace wiz {
class Worker : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "wiz_worker";
  static constexpr MapInteractionType movement_type = MapInteractionType::walk_only_on_flowers;

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

  Result<void, std::string> follow_path(const int64_t delta_time_ns);

  Result<void, std::string> plan();

  std::random_device dev_;
  std::mt19937 rng_{dev_()};
  CharacterMode mode_{CharacterMode::idle};
  Eigen::Vector2f position_{0.f, 0.f};
  float speed_{0.25f};
  Eigen::Vector2f direction_{speed_, speed_};
  bool off_flowers_{false};

  int64_t replan_delay_{500'000'000L};
  int64_t time_since_last_replan_{0L};

  Eigen::Vector2i goal_tile_{30, 30};
  std::optional<std::deque<Eigen::Vector2i>> maybe_current_path_on_tiles_;
};
} // namespace wiz
