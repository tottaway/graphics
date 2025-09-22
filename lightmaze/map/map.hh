#pragma once
#include "model/game_state.hh"
#include "view/screen.hh"
#include <Eigen/Geometry>
#include <chrono>

namespace lightmaze {

class MapEntity;

/**
 * @brief Central map entity that manages all platforms and handles editor mode
 *
 * The Map entity serves as the parent container for all platforms in the level
 * and manages the map editor functionality. It handles:
 * - Editor mode toggling (E key)
 * - Platform creation via left-click drag
 * - Auto-saving every 5 seconds
 * - Save/load integration
 * - Platform management and coordination
 */
class Map : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "lightmaze_map";

  enum class EditorMode { gameplay, editor };

  /**
   * @brief Construct a new Map entity
   * @param game_state Reference to the game state that owns this entity
   */
  Map(model::GameState &game_state);

  /**
   * @brief Initialize the map with default platforms or loaded state
   * @return Ok() on success, Err(message) if initialization fails
   * @pre Entity must be added to game state before calling init()
   * @post Map will attempt to load saved state or create default platforms
   */
  Result<void, std::string> init();

  /**
   * @brief Get the entity type name for identification
   * @return Static string identifying this entity type
   */
  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  };

  /**
   * @brief Check if currently in editor mode
   * @return true if in editor mode, false if in gameplay mode
   */
  bool is_editor_mode() const { return current_mode_ == EditorMode::editor; }

  /**
   * @brief Toggle between gameplay and editor modes
   * @return Ok() on success, Err(message) if toggle fails
   * @post Editor mode state flipped, visual feedback may be updated
   */
  Result<void, std::string> toggle_editor_mode();

  /**
   * @brief Add a new map entity (platform) as a child entity
   * @param top_center_position Position of the platform's top center
   * @param size Size of the platform in meters (width, height)
   * @return EntityID of created map entity on success, Err(message) on failure
   * @post New platform entity created and added as child
   */
  Result<model::EntityID, std::string>
  add_platform(const Eigen::Vector2f &top_center_position,
               const Eigen::Vector2f &size);

  /**
   * @brief Remove a platform by its entity ID
   * @param platform_id EntityID of the platform to remove
   * @return Ok() on success, Err(message) if platform not found or removal
   * fails
   * @post Platform entity removed from game state
   */
  Result<void, std::string> remove_platform(const model::EntityID &platform_id);

  /**
   * @brief Save current map state to YAML file
   * @param file_path Path to save file (default: uses default_save_path)
   * @return Ok() on success, Err(message) if save fails
   * @post Current platform layout and player position saved to YAML file
   */
  Result<void, std::string>
  save_current_state(const std::string &file_path = default_save_path);

  /**
   * @brief Load saved map state from YAML file
   * @param file_path Path to load file (default: uses default_save_path)
   * @return Ok() on success, Err(message) if load fails or file doesn't exist
   * @pre Map should be in initial state (no existing platforms)
   * @post Platforms and player position restored from saved state
   */
  Result<void, std::string>
  load_saved_state(const std::string &file_path = default_save_path);

  /**
   * @brief Handle key press events for editor mode toggle
   * @param key_press Key press event containing which key was pressed
   * @return true if event was handled and should not propagate, false otherwise
   * @post Editor mode may be toggled if E key was pressed
   */
  [[nodiscard]] virtual Result<bool, std::string>
  on_key_press(const view::KeyPressedEvent &key_press);

  /**
   * @brief Handle mouse down events for platform creation
   * @param event Mouse down event with button and position
   * @return true if event was handled and should not propagate, false otherwise
   * @post Platform creation may begin if left-click in editor mode
   */
  [[nodiscard]] virtual Result<bool, std::string>
  on_mouse_down(const view::MouseDownEvent &event);

  /**
   * @brief Handle mouse up events for platform creation completion
   * @param event Mouse up event with button and position
   * @return true if event was handled and should not propagate, false otherwise
   * @post Platform creation may complete if left-click release in editor mode
   */
  [[nodiscard]] virtual Result<bool, std::string>
  on_mouse_up(const view::MouseUpEvent &event);

  /**
   * @brief Handle mouse moved events for platform creation preview
   * @param event Mouse moved event with new position
   * @return true if event was handled and should not propagate, false otherwise
   * @post Platform preview may be updated during creation
   */
  [[nodiscard]] virtual Result<bool, std::string>
  on_mouse_moved(const view::MouseMovedEvent &event);

  /**
   * @brief Update map state including auto-save timer
   * @param delta_time_ns Time elapsed since last update in nanoseconds
   * @return Ok() on success, Err(message) if update fails
   * @pre delta_time_ns should be positive
   * @post Auto-save timer updated, save may occur if interval reached
   */
  [[nodiscard]] virtual Result<void, std::string>
  update(const int64_t delta_time_ns);

  /**
   * @brief Get the z-level for rendering order (background)
   * @return Z-level value (lower values render behind)
   */
  [[nodiscard]] virtual uint8_t get_z_level() const { return 0; }

  /**
   * @brief Get transform covering the entire play area for mouse event
   * detection
   * @return Large transform covering the screen for map editor functionality
   */
  [[nodiscard]] virtual Eigen::Affine2f get_transform() const override;

private:
  /// Default save file path - absolute path to tmp directory
  static constexpr const char *default_save_path{
      "/home/tottaway/projects/graphics/lightmaze/saves/current_level.yaml"};

  /// Current editor mode state
  EditorMode current_mode_{EditorMode::gameplay};

  /// Platform creation state
  bool is_creating_platform_{false};
  Eigen::Vector2f creation_start_pos_;

  /// Auto-save timer
  int64_t time_since_last_save_ns_{0};
  static constexpr int64_t auto_save_interval_ns_{5'000'000'000}; // 5 seconds

  /**
   * @brief Create a platform from start and end positions
   * @param start Start position of drag (world coordinates)
   * @param end End position of drag (world coordinates)
   * @return Ok() on success, Err(message) if creation fails
   * @post New platform entity created and added as child
   */
  Result<void, std::string> create_platform(const Eigen::Vector2f &start,
                                            const Eigen::Vector2f &end);

  /**
   * @brief Check auto-save timer and save if interval elapsed
   * @return Ok() on success, Err(message) if save fails
   * @post Save may occur if enough time has passed since last save
   */
  Result<void, std::string> auto_save_if_needed();
};

} // namespace lightmaze
