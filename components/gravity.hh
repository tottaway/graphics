#pragma once
#include "components/component.hh"
#include <Eigen/Dense>
#include <functional>

namespace component {

/**
 * @brief Generic gravity component that applies acceleration to entities
 *
 * Provides configurable gravitational acceleration that can be applied to any
 * entity. Supports custom acceleration vectors for different gravity directions
 * (downward, upward, sideways, etc.) and allows entities to modify their
 * velocity through callback functions.
 */
class Gravity : public Component {
public:
  static constexpr std::string_view component_type_name = "gravity_component";

  /**
   * @brief Function type for getting current velocity from the entity
   * @return Current velocity vector in meters per second
   */
  using GetVelocityFunc = std::function<Eigen::Vector2f()>;

  /**
   * @brief Function type for setting new velocity on the entity
   * @param velocity New velocity vector in meters per second
   */
  using SetVelocityFunc = std::function<void(const Eigen::Vector2f &velocity)>;

  /**
   * @brief Construct a new Gravity component
   * @param get_velocity_func Function to get current velocity from entity
   * @param set_velocity_func Function to set new velocity on entity
   * @param acceleration_m_per_s2 Acceleration vector in meters per second squared
   * @pre get_velocity_func and set_velocity_func must be valid functions
   * @post Component will apply acceleration to velocity each update
   */
  Gravity(GetVelocityFunc get_velocity_func, SetVelocityFunc set_velocity_func,
          const Eigen::Vector2f &acceleration_m_per_s2 = Eigen::Vector2f{0.0f, -9.8f});

  /**
   * @brief Update gravity by applying acceleration to velocity
   * @param delta_time_ns Time elapsed since last update in nanoseconds
   * @return Ok() on success, Err(message) if velocity functions fail
   * @pre delta_time_ns should be positive
   * @post Entity velocity updated with applied acceleration
   */
  [[nodiscard]] virtual Result<void, std::string>
  update(const int64_t delta_time_ns) override;

  /**
   * @brief Get the component type name for identification
   * @return Static string identifying this component type
   */
  [[nodiscard]] virtual std::string_view get_component_type_name() const override {
    return component_type_name;
  }

  /**
   * @brief Set new gravitational acceleration
   * @param acceleration_m_per_s2 New acceleration vector in meters per second squared
   * @post Future updates will use the new acceleration value
   */
  void set_acceleration(const Eigen::Vector2f &acceleration_m_per_s2);

  /**
   * @brief Get current gravitational acceleration
   * @return Current acceleration vector in meters per second squared
   */
  [[nodiscard]] const Eigen::Vector2f &get_acceleration() const;

  /**
   * @brief Enable or disable gravity application
   * @param enabled Whether gravity should be applied during updates
   * @post Gravity will only be applied when enabled is true
   */
  void set_enabled(bool enabled);

  /**
   * @brief Check if gravity is currently enabled
   * @return true if gravity is being applied, false otherwise
   */
  [[nodiscard]] bool is_enabled() const;

private:
  /// Function to get current velocity from the owning entity
  GetVelocityFunc get_velocity_func_;

  /// Function to set new velocity on the owning entity
  SetVelocityFunc set_velocity_func_;

  /// Gravitational acceleration vector in meters per second squared
  Eigen::Vector2f acceleration_m_per_s2_;

  /// Whether gravity is currently enabled
  bool enabled_{true};
};

} // namespace component