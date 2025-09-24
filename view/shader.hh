#pragma once
#include "utility/try.hh"
#include <Eigen/Dense>
#include <GL/glew.h>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace view {

/**
 * @brief GLSL shader program wrapper with uniform management
 *
 * Provides a simple interface for loading, compiling, and using GLSL shaders.
 * Handles vertex and fragment shader compilation, linking, and uniform uploads.
 */
class Shader {
public:
  /**
   * @brief Load and compile vertex + fragment shader from files
   * @param vertex_path Path to vertex shader file (.vert)
   * @param fragment_path Path to fragment shader file (.frag)
   * @return Compiled shader program on success, error message on failure
   */
  [[nodiscard]] static Result<std::unique_ptr<Shader>, std::string>
  from_files(const std::string& vertex_path, const std::string& fragment_path);

  /**
   * @brief Create shader from source strings (for testing)
   * @param vertex_source Vertex shader GLSL source code
   * @param fragment_source Fragment shader GLSL source code
   * @return Compiled shader program on success, error message on failure
   */
  [[nodiscard]] static Result<std::unique_ptr<Shader>, std::string>
  from_strings(const std::string& vertex_source, const std::string& fragment_source);

  /**
   * @brief Activate this shader for rendering
   * @post All subsequent OpenGL drawing calls will use this shader
   */
  void use() const;

  /**
   * @brief Set uniform values for shader parameters
   */
  void set_uniform(const std::string& name, float value);
  void set_uniform(const std::string& name, const Eigen::Vector2f& value);
  void set_uniform(const std::string& name, const Eigen::Vector3f& value);
  void set_uniform(const std::string& name, int32_t value);

  /**
   * @brief Set array uniforms for multiple values (e.g. light data)
   */
  void set_uniform_array(const std::string& name, const std::vector<Eigen::Vector2f>& values);
  void set_uniform_array(const std::string& name, const std::vector<Eigen::Vector3f>& values);
  void set_uniform_array(const std::string& name, const std::vector<float>& values);

  /**
   * @brief Check if shader is valid and ready to use
   * @return True if shader compiled and linked successfully
   */
  [[nodiscard]] bool is_valid() const { return program_id_ != 0; }

  /**
   * @brief Get OpenGL program ID for advanced usage
   * @return OpenGL program handle
   */
  [[nodiscard]] uint32_t get_program_id() const { return program_id_; }

  // Move-only type (shaders are GPU resources)
  Shader(const Shader&) = delete;
  Shader& operator=(const Shader&) = delete;
  Shader(Shader&& other) noexcept;
  Shader& operator=(Shader&& other) noexcept;

  ~Shader();

private:
  explicit Shader(uint32_t program_id) : program_id_(program_id) {}

  uint32_t program_id_{0};
  std::unordered_map<std::string, int32_t> uniform_cache_;

  /**
   * @brief Get cached uniform location
   * @param name Uniform variable name
   * @return OpenGL uniform location, -1 if not found
   */
  int32_t get_uniform_location(const std::string& name);

  /**
   * @brief Compile a single shader stage
   * @param source GLSL source code
   * @param shader_type GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
   * @return Compiled shader ID on success, error message on failure
   */
  [[nodiscard]] static Result<uint32_t, std::string>
  compile_shader(const std::string& source, uint32_t shader_type);

  /**
   * @brief Link vertex and fragment shaders into program
   * @param vertex_shader Compiled vertex shader ID
   * @param fragment_shader Compiled fragment shader ID
   * @return Linked program ID on success, error message on failure
   */
  [[nodiscard]] static Result<uint32_t, std::string>
  link_program(uint32_t vertex_shader, uint32_t fragment_shader);
};

} // namespace view