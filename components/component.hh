#pragma once
#include "utility/try.hh"
#include "view/screen.hh"
namespace component {
class Component {
public:
  virtual ~Component() = default;

  /// Draw the entity on the screen
  /// @param[in] screen object used to interact with the screen
  [[nodiscard]] virtual Result<void, std::string>
  draw(view::Screen &screen) const {
    return Ok();
  }

  /// Update the internal state of the Enitity
  /// @param[in] timestamp_ns the current time in nanoseconds
  [[nodiscard]] virtual Result<void, std::string>
  update(const int64_t delta_time_ns) {
    return Ok();
  }

  /// TODO idk what this is for
  /// @param[in] timestamp_ns the current time in nanoseconds
  [[nodiscard]] virtual Result<void, std::string> late_update() { return Ok(); }

  /// get a string identitying what type of component this is
  /// @return string identitying what type of component this is
  [[nodiscard]] virtual std::string_view get_component_type_name() const = 0;
};
} // namespace component
