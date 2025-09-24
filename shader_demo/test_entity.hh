#pragma once
#include "model/game_state.hh"
#include "utility/try.hh"
#include <Eigen/Dense>

namespace shader_demo {

/**
 * @brief Simple test entity that uses ShaderRenderer component
 *
 * This entity demonstrates:
 * - Using the ShaderRenderer component for custom shader effects
 * - Clean separation between entity logic and rendering
 * - Reusable component architecture
 */
class TestEntity : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "test_entity";

  // Shader file paths
  static constexpr std::string_view vertex_shader_path = "shader_demo/assets/shaders/basic.vert";
  static constexpr std::string_view fragment_shader_path = "shader_demo/assets/shaders/basic.frag";

  /**
   * @brief Construct test entity
   * @param game_state Reference to game state
   */
  explicit TestEntity(model::GameState& game_state);

  /**
   * @brief Initialize the entity and add shader component
   * @param position World position for this test entity
   * @return Ok() on success, error message if initialization fails
   */
  [[nodiscard]] Result<void, std::string> init(const Eigen::Vector2f& position = {0.0f, 0.0f});

  [[nodiscard]] std::string_view get_entity_type_name() const override {
    return entity_type_name;
  }

  /**
   * @brief Get entity transform for component callbacks
   * @return Transform matrix with current position and identity scaling/rotation
   */
  [[nodiscard]] Eigen::Affine2f get_transform() const;

  /**
   * @brief Get current world position
   * @return Entity position in world coordinates
   */
  [[nodiscard]] Eigen::Vector2f get_position() const { return position_; }

  /**
   * @brief Set new world position (for testing movement)
   * @param new_position New world position
   */
  void set_position(const Eigen::Vector2f& new_position) { position_ = new_position; }

private:
  Eigen::Vector2f position_{0.0f, 0.0f};
};

} // namespace shader_demo