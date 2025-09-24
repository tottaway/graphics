#pragma once
#include "components/component.hh"
#include "view/screen.hh"
#include "view/shader.hh"
#include "utility/try.hh"
#include <Eigen/Dense>
#include <functional>
#include <memory>
#include <unordered_map>
#include <string>

namespace component {

/**
 * @brief Component for rendering fullscreen effects using custom shaders
 *
 * This component allows entities to render custom shader effects without
 * needing to know about OpenGL or shader management. The component handles:
 * - Shader loading and compilation
 * - Uniform parameter management
 * - Fullscreen quad rendering
 * - Animation and time-based effects
 */
class ShaderRenderer : public Component {
public:
  /**
   * @brief Function type for providing dynamic uniform values
   * Called each frame to get current uniform values
   */
  using UniformProvider = std::function<void(view::Shader&)>;

  /**
   * @brief Parameters for creating a shader renderer
   */
  struct ShaderParams {
    std::string vertex_shader_path;     ///< Path to vertex shader file
    std::string fragment_shader_path;   ///< Path to fragment shader file
    UniformProvider uniform_provider;   ///< Function to set shader uniforms each frame
    float z_level{0.0f};               ///< Z-level for depth sorting
  };

  /**
   * @brief Construct shader renderer with file paths
   * @param params Shader configuration parameters
   */
  explicit ShaderRenderer(const ShaderParams& params);

  static constexpr std::string_view component_type_name = "shader_renderer";

  [[nodiscard]] std::string_view get_component_type_name() const override {
    return component_type_name;
  }

  /**
   * @brief Update animation time for shader uniforms
   * @param delta_time_ns Time since last update in nanoseconds
   * @return Ok() on success, error message on failure
   */
  [[nodiscard]] Result<void, std::string> update(const int64_t delta_time_ns) override;

  /**
   * @brief Render the shader effect
   * @param screen Screen to render to
   * @return Ok() on success, error message on shader errors
   */
  [[nodiscard]] Result<void, std::string> draw(view::Screen& screen) const override;

  /**
   * @brief Get the z-level for this shader effect
   * @return Z-level value for depth sorting
   */
  [[nodiscard]] float get_z_level() const { return z_level_; }

  /**
   * @brief Set the z-level for this shader effect
   * @param new_z_level New z-level value
   */
  void set_z_level(float new_z_level) { z_level_ = new_z_level; }

  /**
   * @brief Check if shader is loaded and ready
   * @return True if shader compiled successfully
   */
  [[nodiscard]] bool is_shader_loaded() const;

private:
  std::unique_ptr<view::Shader> shader_;
  UniformProvider uniform_provider_;
  float z_level_;

  // Shader loading parameters (stored for lazy loading)
  std::string vertex_shader_path_;
  std::string fragment_shader_path_;
  bool shader_load_attempted_{false};

  /**
   * @brief Lazy-load shader on first draw call
   * @return Ok() on success, error message if loading fails
   */
  Result<void, std::string> ensure_shader_loaded();

  /**
   * @brief Render a fullscreen quad using the current shader
   * @param screen Screen to render to
   * @return Ok() on success, error on OpenGL issues
   */
  Result<void, std::string> render_fullscreen_quad(view::Screen& screen);
};


} // namespace component