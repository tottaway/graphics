#pragma once
#include "model/game_state.hh"
#include "utility/try.hh"

namespace wiz {
class WizModeManager : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "wiz_mode_manager";

  virtual ~WizModeManager() = default;
  WizModeManager(model::GameState &game_state);

  /// Update the internal state of the Enitity
  /// @param[in] delta_time_ns the current time in nanoseconds
  [[nodiscard]] virtual Result<void, std::string>
  update(const int64_t delta_time_ns);

  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  };

private:
  enum class GameMode { init, in_game, dead };
  [[nodiscard]] Result<void, std::string> start_new_game();
  GameMode game_mode_{GameMode::init};
};
} // namespace wiz
