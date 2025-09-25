#pragma once
#include "components/component.hh"
#include "view/screen.hh"
#include <Eigen/Geometry>
#include <functional>
#include <limits>
#include <memory>

namespace component {

/**
 * @brief Base class for different light geometries
 */
class LightGeometry {
public:
  virtual ~LightGeometry() = default;

  /**
   * @brief Get the type name for this geometry
   * @return String identifying the geometry type
   */
  [[nodiscard]] virtual std::string_view get_geometry_type() const = 0;


  /**
   * @brief Get the bounding radius for optimization
   * @return Maximum distance from center that this light can affect
   */
  [[nodiscard]] virtual float get_bounding_radius() const = 0;
};

/**
 * @brief Global light geometry that illuminates everywhere uniformly
 */
class GlobalLightGeometry : public LightGeometry {
public:
  /// Geometry type identifier for global lights
  static constexpr std::string_view geometry_type_name = "global";

  /**
   * @brief Construct global light geometry
   * @post Light provides uniform illumination everywhere
   */
  GlobalLightGeometry() = default;

  [[nodiscard]] std::string_view get_geometry_type() const override {
    return geometry_type_name;
  }


  [[nodiscard]] float get_bounding_radius() const override {
    // Global light affects infinite radius
    return std::numeric_limits<float>::max();
  }
};

/**
 * @brief Circular light geometry with radius-based falloff
 */
class CircularLightGeometry : public LightGeometry {
public:
  /// Geometry type identifier for circular lights
  static constexpr std::string_view geometry_type_name = "circular";

  /**
   * @brief Construct circular light geometry
   * @param radius_meters Light radius in meters
   * @pre radius_meters > 0.0f
   */
  explicit CircularLightGeometry(float radius_meters);

  [[nodiscard]] std::string_view get_geometry_type() const override {
    return geometry_type_name;
  }


  [[nodiscard]] float get_bounding_radius() const override {
    return radius_meters_;
  }

  /**
   * @brief Set the radius of the circular light
   * @param new_radius_meters New radius in meters
   * @pre new_radius_meters > 0.0f
   */
  void set_radius(float new_radius_meters);

  /**
   * @brief Get the current radius
   * @return Current radius in meters
   */
  [[nodiscard]] float get_radius() const { return radius_meters_; }

private:
  float radius_meters_;
};

/**
 * @brief Generic light source component supporting multiple geometries
 *
 * LightEmitter creates a light source at the entity's transform position.
 * The light geometry determines the shape and falloff pattern, while
 * color and intensity provide the visual characteristics.
 */
class LightEmitter : public Component {
public:
  /**
   * @brief Information about the current light state
   */
  struct LightInfo {
    Eigen::Vector2f world_position;           ///< Center position in world coordinates
    std::shared_ptr<LightGeometry> geometry;  ///< Light geometry (shape and falloff)
    view::Color color;                        ///< Light color (RGB)
    float intensity;                          ///< Overall light intensity (0.0-1.0)
  };

  /**
   * @brief Function type for getting entity transform
   */
  using TransformFunc = std::function<Eigen::Affine2f()>;

  /**
   * @brief Parameters for creating a circular light
   */
  struct CircularLightParams {
    TransformFunc transform_func;
    float radius_meters;
    view::Color color;
    float intensity;
  };

  /**
   * @brief Parameters for creating a global light
   */
  struct GlobalLightParams {
    view::Color color;
    float intensity;
  };

  /**
   * @brief Parameters for creating a light with custom geometry
   */
  struct CustomGeometryLightParams {
    TransformFunc transform_func;
    std::shared_ptr<LightGeometry> geometry;
    view::Color color;
    float intensity;
  };

  /**
   * @brief Construct a circular light emitter
   * @param params Parameters for circular light configuration
   * @pre params.radius_meters > 0.0f
   * @pre params.intensity >= 0.0f && params.intensity <= 1.0f
   */
  explicit LightEmitter(const CircularLightParams& params);

  /**
   * @brief Construct a global light emitter
   * @param params Parameters for global light configuration
   * @pre params.intensity >= 0.0f && params.intensity <= 1.0f
   */
  explicit LightEmitter(const GlobalLightParams& params);

  /**
   * @brief Construct a light emitter with custom geometry
   * @param params Parameters for custom geometry light configuration
   * @pre params.geometry != nullptr
   * @pre params.intensity >= 0.0f && params.intensity <= 1.0f
   */
  explicit LightEmitter(const CustomGeometryLightParams& params);

  static constexpr std::string_view component_type_name = "light_emitter_component";

  /**
   * @brief Get the component type name for identification
   * @return Static string identifying this component type
   */
  [[nodiscard]] virtual std::string_view get_component_type_name() const override {
    return component_type_name;
  }

  /**
   * @brief Get current light information
   * @return LightInfo struct with current position, geometry, color, and intensity
   * @post Uses transform_func to get current entity position
   */
  [[nodiscard]] LightInfo get_light_info() const;

  /**
   * @brief Set light intensity (useful for flickering, dimming effects)
   * @param new_intensity New intensity value (0.0-1.0)
   * @pre new_intensity >= 0.0f && new_intensity <= 1.0f
   */
  void set_intensity(float new_intensity);

  /**
   * @brief Set light color (useful for dynamic color changes)
   * @param new_color New light color
   */
  void set_color(const view::Color& new_color);

  /**
   * @brief Replace the light geometry
   * @param new_geometry New geometry for this light
   * @pre new_geometry != nullptr
   */
  void set_geometry(std::shared_ptr<LightGeometry> new_geometry);

private:
  /// Function to get entity's current transform
  TransformFunc transform_func_;

  /// Light geometry (shape and falloff)
  std::shared_ptr<LightGeometry> geometry_;

  /// Light color
  view::Color color_;

  /// Light intensity (0.0-1.0)
  float intensity_;
};

} // namespace component