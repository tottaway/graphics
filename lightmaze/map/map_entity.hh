#pragma once
#include "model/game_state.hh"
#include <yaml-cpp/yaml.h>
#include <Eigen/Geometry>
#include <variant>
#include <chrono>

namespace lightmaze {

/**
 * @brief Represents a platform entity with editor manipulation capabilities
 *
 * MapEntity currently implements platform functionality with support for:
 * - Solid collision detection for player physics
 * - Jump reset detection on platform tops
 * - Right-click drag manipulation in editor mode
 * - Double right-click deletion in editor mode
 * - YAML serialization for save/load functionality
 *
 * The entity is designed to be extensible for future map object types through
 * the variant-based EntityParams system.
 */
class MapEntity : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "lightmaze_map_entity";

  /**
   * @brief Parameters for platform entities
   */
  struct PlatformParams {
    Eigen::Vector2f top_center_position{0.0f, 0.0f};
    Eigen::Vector2f size{2.0f, 0.2f};
  };

  /**
   * @brief Parameters for deserializing a MapEntity from YAML
   */
  struct SerializedMapEntityParams {
    YAML::Node yaml_node;
  };

  /**
   * @brief Variant containing all possible entity type parameters
   */
  using EntityParams = std::variant<PlatformParams, SerializedMapEntityParams>;

  /**
   * @brief Parameters for initializing a MapEntity
   */
  struct Params {
    EntityParams entity_params;
  };

  /**
   * @brief Construct a new MapEntity
   * @param game_state Reference to the game state that owns this entity
   */
  MapEntity(model::GameState &game_state);

  /**
   * @brief Initialize the MapEntity with the specified parameters
   * @param params Configuration parameters determining entity type and properties
   * @return Ok() on success, Err(message) if initialization fails
   * @post Entity configured with components appropriate for the specified type
   */
  Result<void, std::string> init(const Params &params);

  /**
   * @brief Get the entity type name for identification
   * @return Static string identifying this entity type
   */
  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  }

  /**
   * @brief Update the entity state
   * @param delta_time_ns Time elapsed since last update in nanoseconds
   * @return Ok() on success, Err(message) if update fails
   */
  [[nodiscard]] virtual Result<void, std::string>
  update(const int64_t delta_time_ns);

  /**
   * @brief Get the current transform matrix for rendering
   * @return Affine transform representing position and scale
   */
  [[nodiscard]] virtual Eigen::Affine2f get_transform() const;

  /**
   * @brief Get the z-level for rendering order
   * @return Z-level value (lower values render behind)
   */
  [[nodiscard]] virtual uint8_t get_z_level() const { return 0; }

  /**
   * @brief Get the top center position of the entity
   * @return World coordinates of the entity's top center point
   */
  [[nodiscard]] Eigen::Vector2f get_top_center_position() const;


  /**
   * @brief Get the entity size based on its transform
   * @return Size vector (width, height) in meters calculated from transform bounds
   */
  [[nodiscard]] Eigen::Vector2f get_size() const;

  /**
   * @brief Serialize this entity to a YAML node
   * @return YAML node containing all data needed to recreate this entity
   */
  [[nodiscard]] YAML::Node serialize() const;

  /**
   * @brief Handle mouse down events for platform manipulation
   * @param event Mouse down event with button and position
   * @return true if event was handled and should not propagate, false otherwise
   * @post Platform dragging may begin if right-click on this platform in editor mode
   */
  [[nodiscard]] virtual Result<bool, std::string>
  on_mouse_down(const view::MouseDownEvent &event);

  /**
   * @brief Handle mouse up events for platform manipulation
   * @param event Mouse up event with button and position
   * @return true if event was handled and should not propagate, false otherwise
   * @post Platform dragging may end or deletion may occur with double right-click
   */
  [[nodiscard]] virtual Result<bool, std::string>
  on_mouse_up(const view::MouseUpEvent &event);

  /**
   * @brief Handle mouse moved events for platform dragging
   * @param event Mouse moved event with new position
   * @return true if event was handled and should not propagate, false otherwise
   * @post Platform position may be updated during dragging
   */
  [[nodiscard]] virtual Result<bool, std::string>
  on_mouse_moved(const view::MouseMovedEvent &event);

  /**
   * @brief Set the platform's position (updates the top center position)
   * @param new_top_center_position New position for the platform's top center
   * @post Platform position updated and internal state synchronized
   */
  void set_position(const Eigen::Vector2f &new_top_center_position);


private:
  /// Current entity parameters
  EntityParams entity_params_;

  /// Entity position in world coordinates (calculated from params)
  Eigen::Vector2f position_{0.0f, 0.0f};

  /// Platform manipulation state
  bool is_being_dragged_{false};
  Eigen::Vector2f drag_offset_;

  /// Double-click detection for deletion
  std::chrono::steady_clock::time_point last_right_click_time_;
  static constexpr int64_t double_click_threshold_ns_{400'000'000}; // 400ms

  /**
   * @brief Initialize as a platform entity
   * @param platform_params Platform-specific parameters
   * @return Ok() on success, Err(message) if initialization fails
   */
  Result<void, std::string> init_as_platform(const PlatformParams& platform_params);

  /**
   * @brief Initialize from YAML node data
   * @param yaml_node YAML node containing entity data
   * @return Ok() on success, Err(message) if initialization fails
   */
  Result<void, std::string> init_from_yaml(const YAML::Node& yaml_node);
};

} // namespace lightmaze