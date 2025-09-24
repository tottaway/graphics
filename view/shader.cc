#include "view/shader.hh"
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

namespace view {

Result<std::unique_ptr<Shader>, std::string> Shader::from_files(const std::string& vertex_path,
                                                                  const std::string& fragment_path) {
  // Read vertex shader source
  std::ifstream vertex_file(vertex_path);
  if (!vertex_file.is_open()) {
    return Err(std::string("Failed to open vertex shader file: ") + vertex_path);
  }

  std::stringstream vertex_stream;
  vertex_stream << vertex_file.rdbuf();
  const std::string vertex_source = vertex_stream.str();
  vertex_file.close();

  // Read fragment shader source
  std::ifstream fragment_file(fragment_path);
  if (!fragment_file.is_open()) {
    return Err(std::string("Failed to open fragment shader file: ") + fragment_path);
  }

  std::stringstream fragment_stream;
  fragment_stream << fragment_file.rdbuf();
  const std::string fragment_source = fragment_stream.str();
  fragment_file.close();

  return from_strings(vertex_source, fragment_source);
}

Result<std::unique_ptr<Shader>, std::string> Shader::from_strings(const std::string& vertex_source,
                                                                    const std::string& fragment_source) {
  // Check if GLEW is initialized
  if (!glewIsSupported("GL_VERSION_2_0")) {
    return Err(std::string("OpenGL 2.0+ not supported or GLEW not initialized"));
  }

  // Compile vertex shader
  const auto vertex_shader_result = compile_shader(vertex_source, GL_VERTEX_SHADER);
  if (vertex_shader_result.isErr()) {
    return Err(std::string("Vertex shader compilation failed: ") + vertex_shader_result.unwrapErr());
  }
  const uint32_t vertex_shader = vertex_shader_result.unwrap();

  // Compile fragment shader
  const auto fragment_shader_result = compile_shader(fragment_source, GL_FRAGMENT_SHADER);
  if (fragment_shader_result.isErr()) {
    glDeleteShader(vertex_shader);  // Cleanup on failure
    return Err(std::string("Fragment shader compilation failed: ") + fragment_shader_result.unwrapErr());
  }
  const uint32_t fragment_shader = fragment_shader_result.unwrap();

  // Link program
  const auto program_result = link_program(vertex_shader, fragment_shader);

  // Clean up individual shaders (no longer needed after linking)
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  if (program_result.isErr()) {
    return Err(std::string("Shader program linking failed: ") + program_result.unwrapErr());
  }

  return Ok(std::unique_ptr<Shader>(new Shader(program_result.unwrap())));
}

void Shader::use() const {
  if (program_id_ != 0) {
    glUseProgram(program_id_);
  }
}

void Shader::set_uniform(const std::string& name, float value) {
  use();
  const int32_t location = get_uniform_location(name);
  if (location != -1) {
    glUniform1f(location, value);
  }
}

void Shader::set_uniform(const std::string& name, const Eigen::Vector2f& value) {
  use();
  const int32_t location = get_uniform_location(name);
  if (location != -1) {
    glUniform2f(location, value.x(), value.y());
  }
}

void Shader::set_uniform(const std::string& name, const Eigen::Vector3f& value) {
  use();
  const int32_t location = get_uniform_location(name);
  if (location != -1) {
    glUniform3f(location, value.x(), value.y(), value.z());
  }
}

void Shader::set_uniform(const std::string& name, int32_t value) {
  use();
  const int32_t location = get_uniform_location(name);
  if (location != -1) {
    glUniform1i(location, value);
  }
}

void Shader::set_uniform_array(const std::string& name, const std::vector<Eigen::Vector2f>& values) {
  if (values.empty()) return;

  use();
  const int32_t location = get_uniform_location(name);
  if (location != -1) {
    // Convert Eigen vectors to float array
    std::vector<float> float_data;
    float_data.reserve(values.size() * 2);
    for (const auto& v : values) {
      float_data.push_back(v.x());
      float_data.push_back(v.y());
    }
    glUniform2fv(location, static_cast<int32_t>(values.size()), float_data.data());
  }
}

void Shader::set_uniform_array(const std::string& name, const std::vector<Eigen::Vector3f>& values) {
  if (values.empty()) return;

  use();
  const int32_t location = get_uniform_location(name);
  if (location != -1) {
    // Convert Eigen vectors to float array
    std::vector<float> float_data;
    float_data.reserve(values.size() * 3);
    for (const auto& v : values) {
      float_data.push_back(v.x());
      float_data.push_back(v.y());
      float_data.push_back(v.z());
    }
    glUniform3fv(location, static_cast<int32_t>(values.size()), float_data.data());
  }
}

void Shader::set_uniform_array(const std::string& name, const std::vector<float>& values) {
  if (values.empty()) return;

  use();
  const int32_t location = get_uniform_location(name);
  if (location != -1) {
    glUniform1fv(location, static_cast<int32_t>(values.size()), values.data());
  }
}

Shader::Shader(Shader&& other) noexcept : program_id_(other.program_id_), uniform_cache_(std::move(other.uniform_cache_)) {
  other.program_id_ = 0;  // Mark other as invalid
}

Shader& Shader::operator=(Shader&& other) noexcept {
  if (this != &other) {
    // Clean up current resources
    if (program_id_ != 0) {
      glDeleteProgram(program_id_);
    }

    // Take ownership from other
    program_id_ = other.program_id_;
    uniform_cache_ = std::move(other.uniform_cache_);

    // Mark other as invalid
    other.program_id_ = 0;
  }
  return *this;
}

Shader::~Shader() {
  if (program_id_ != 0) {
    glDeleteProgram(program_id_);
  }
}

int32_t Shader::get_uniform_location(const std::string& name) {
  // Check cache first
  const auto it = uniform_cache_.find(name);
  if (it != uniform_cache_.end()) {
    return it->second;
  }

  // Query OpenGL and cache result
  const int32_t location = glGetUniformLocation(program_id_, name.c_str());
  uniform_cache_[name] = location;

  if (location == -1) {
    std::cerr << "Warning: Uniform '" << name << "' not found in shader program" << std::endl;
  }

  return location;
}

Result<uint32_t, std::string> Shader::compile_shader(const std::string& source, uint32_t shader_type) {
  const uint32_t shader = glCreateShader(shader_type);
  if (shader == 0) {
    return Err(std::string("glCreateShader failed - OpenGL error"));
  }

  const char* source_cstr = source.c_str();
  glShaderSource(shader, 1, &source_cstr, nullptr);
  glCompileShader(shader);

  // Check compilation status
  int32_t success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  if (!success) {
    char info_log[512];
    glGetShaderInfoLog(shader, 512, nullptr, info_log);
    glDeleteShader(shader);

    const std::string shader_type_name = (shader_type == GL_VERTEX_SHADER) ? "vertex" : "fragment";
    return Err(std::string("Shader compilation error (") + shader_type_name + "): " + info_log);
  }

  return Ok(shader);
}

Result<uint32_t, std::string> Shader::link_program(uint32_t vertex_shader, uint32_t fragment_shader) {
  const uint32_t program = glCreateProgram();

  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  // Check linking status
  int32_t success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);

  if (!success) {
    char info_log[512];
    glGetProgramInfoLog(program, 512, nullptr, info_log);
    glDeleteProgram(program);

    return Err(std::string("Program linking error: ") + info_log);
  }

  return Ok(program);
}

} // namespace view