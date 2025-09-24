#include "components/shader_renderer.hh"
#include <iostream>

namespace component {

ShaderRenderer::ShaderRenderer(const ShaderParams& params)
    : uniform_provider_(params.uniform_provider),
      z_level_(params.z_level),
      vertex_shader_path_(params.vertex_shader_path),
      fragment_shader_path_(params.fragment_shader_path) {
  // Shader loading is deferred to first draw call when OpenGL context is ready
}

Result<void, std::string> ShaderRenderer::update(const int64_t delta_time_ns) {
  // No state to update - uniforms are set fresh each frame
  return Ok();
}

Result<void, std::string> ShaderRenderer::draw(view::Screen& screen) const {
  // Lazy-load shader on first draw call (when OpenGL context is ready)
  auto* mutable_this = const_cast<ShaderRenderer*>(this);
  TRY_VOID(mutable_this->ensure_shader_loaded());

  if (!shader_ || !shader_->is_valid()) {
    // Skip rendering if shader failed to load
    return Ok();
  }

  // Set up shader uniforms
  if (uniform_provider_) {
    uniform_provider_(*shader_);
  }

  // Delegate actual rendering to Screen class
  screen.draw_fullscreen_shader(*shader_, z_level_);

  return Ok();
}

bool ShaderRenderer::is_shader_loaded() const {
  return shader_ && shader_->is_valid();
}

Result<void, std::string> ShaderRenderer::ensure_shader_loaded() {
  // Only attempt loading once
  if (shader_load_attempted_) {
    return Ok();
  }

  shader_load_attempted_ = true;

  // Load shader from files
  auto shader_result = view::Shader::from_files(vertex_shader_path_, fragment_shader_path_);

  if (shader_result.isOk()) {
    shader_ = std::move(shader_result).unwrap();
  } else {
    std::cerr << "Failed to load shader: " << shader_result.unwrapErr() << std::endl;
    // Component will be non-functional but won't crash
  }

  return Ok();
}


} // namespace component