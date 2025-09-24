#include "shader_demo/test_entity.hh"
#include "components/shader_renderer.hh"
#include <iostream>

namespace shader_demo {

TestEntity::TestEntity(model::GameState& game_state)
    : Entity(game_state) {
}

Result<void, std::string> TestEntity::init(const Eigen::Vector2f& position) {
  position_ = position;
  std::cout << "Initializing TestEntity with shader renderer..." << std::endl;

  // Create shader renderer component with a simple lighting test
  const component::ShaderRenderer::ShaderParams shader_params{
    .vertex_shader_path = std::string(vertex_shader_path),
    .fragment_shader_path = std::string(fragment_shader_path),
    .uniform_provider = [this](view::Shader& shader) {
      // Get entity position as light position
      const Eigen::Affine2f transform = get_transform();
      const Eigen::Vector2f position = transform.translation();

      // Set viewport parameters (these should come from screen/camera)
      shader.set_uniform("viewport_center", Eigen::Vector2f{0.0f, 0.0f});
      shader.set_uniform("viewport_size", Eigen::Vector2f{4.0f, 4.0f});

      // Set light parameters
      shader.set_uniform("test_light_pos", position);
      shader.set_uniform("test_light_color", Eigen::Vector3f{1.0f, 0.8f, 0.6f});
    },
    .z_level = -1.0f  // Behind other entities
  };

  // Add the shader renderer component to this entity
  add_component<component::ShaderRenderer>(shader_params);

  std::cout << "TestEntity initialized successfully." << std::endl;
  return Ok();
}

Eigen::Affine2f TestEntity::get_transform() const {
  // Create transform with current position and identity scaling/rotation
  Eigen::Affine2f transform = Eigen::Affine2f::Identity();
  transform.translate(position_);
  return transform;
}

} // namespace shader_demo