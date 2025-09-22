#pragma once
#include "model/game_state.hh"
#include <yaml-cpp/yaml.h>
#include <Eigen/Geometry>
#include <variant>

namespace lightmaze {

/**
 * @brief Configurable entity that can represent different map objects (platforms, walls, etc.)
 *
 * MapEntity is a flexible entity that can be configured to represent different types
 * of map objects through its initialization parameters. The entity type determines
 * which components are added and how the entity behaves.
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
   * @brief Get the current entity parameters
   * @return The EntityParams this MapEntity was initialized with
   */
  [[nodiscard]] const EntityParams& get_entity_params() const { return entity_params_; }

  /**
   * @brief Get the current size (works for platform entities)
   * @return Size vector (width, height) in meters
   */
  [[nodiscard]] Eigen::Vector2f get_size() const;

  /**
   * @brief Serialize this entity to a YAML node
   * @return YAML node containing all data needed to recreate this entity
   */
  [[nodiscard]] YAML::Node serialize() const;


private:
  /// Current entity parameters
  EntityParams entity_params_;

  /// Entity position in world coordinates (calculated from params)
  Eigen::Vector2f position_{0.0f, 0.0f};

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