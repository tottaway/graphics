#pragma once
#include "components/component.hh"
#include "model/entity_id.hh"
#include "systems/system.hh"
#include "utility/try.hh"
#include "view/screen.hh"
#include <Eigen/Dense>
#include <array>
#include <cstdint>
#include <limits>
#include <memory>
#include <type_traits>
#include <unordered_set>

namespace model {
class GameState;

class Entity {
public:
  virtual ~Entity();
  /// Construct a Entity
  /// All derived class should avoid doing work inside of the constructor and
  /// favor using an init method, this is because if we create child entities
  /// before construction is finished we won't have a entity id yet
  /// @param[in] game_state class holding the full state of the game
  Entity(GameState &game_state);

  /// Draw the entity on the screen
  /// @param[in] screen object used to interact with the screen
  [[nodiscard]] virtual Result<void, std::string>
  draw(view::Screen &screen) const;

  /// Update the internal state of the Enitity
  /// @param[in] delta_time_ns the current time in nanoseconds
  [[nodiscard]] virtual Result<void, std::string>
  update(const int64_t delta_time_ns) {
    return Ok();
  }

  /// Update the internal state of the Enitity again after systems run
  /// @param[in] delta_time_ns the current time in nanoseconds
  [[nodiscard]] virtual Result<void, std::string> late_update() { return Ok(); }

  /// Generic handle event function
  /// In general this should be avoided in favor of specific handlers like
  /// `on_click`
  /// @param[in] event event originating from screen
  [[nodiscard]] virtual Result<void, std::string>
  handle_event(const view::EventType &event) {
    return Ok();
  }

  /// Handler for click (defined as mouse up)
  /// @param[in] mouse_up underlying event from the screen
  /// @return true if we should keep passing this event to other entities, false
  /// otherwise, propagates any error which occur while handling the event
  [[nodiscard]] virtual Result<bool, std::string>
  on_click(const view::MouseUpEvent &mouse_up) {
    return Ok(true);
  }

  /// Handler for key press
  /// @param[in] key_press underlying event from the screen
  /// @return true if we should keep passing this event to other entities, false
  /// otherwise, propagates any error which occur while handling the event
  [[nodiscard]] virtual Result<bool, std::string>
  on_key_press(const view::KeyPressedEvent &key_press) {
    return Ok(true);
  }

  /// Handler for key release TODO: move this into components
  /// @param[in] key_release underlying event from the screen
  /// @return true if we should keep passing this event to other entities, false
  /// otherwise, propagates any error which occur while handling the event
  [[nodiscard]] virtual Result<bool, std::string>
  on_key_release(const view::KeyReleasedEvent &key_release) {
    return Ok(true);
  }

  /// Return the bounding box for this entity
  /// @return optional bounding box if this handler is implemented
  [[nodiscard]] virtual Eigen::Affine2f get_transform() const {
    return Eigen::Affine2f::Identity();
  };

  template <
      typename ComponentType,
      typename std::enable_if_t<
          std::is_base_of_v<component::Component, ComponentType>, int> = 0>
  [[nodiscard]] std::optional<ComponentType *> get_component() const;

  template <
      typename ComponentType,
      typename std::enable_if_t<
          std::is_base_of_v<component::Component, ComponentType>, int> = 0>
  [[nodiscard]] std::vector<ComponentType *> get_components() const;

  template <
      typename ComponentType,
      typename std::enable_if_t<
          std::is_base_of_v<component::Component, ComponentType>, int> = 0>
  void remove_components() const;

  /// Get the name of the entity type
  /// @note all Entities should have a static member called `entity_type_name`
  /// which this returns
  /// TODO: enfornce this through some type of CRTP or something
  /// @return name of this entity type
  [[nodiscard]] virtual std::string_view get_entity_type_name() const = 0;

  template <
      typename EntityType,
      typename std::enable_if_t<std::is_base_of_v<Entity, EntityType>, int> = 0>
  [[nodiscard]] bool is() const;

  template <
      typename EntityType,
      typename std::enable_if_t<std::is_base_of_v<Entity, EntityType>, int> = 0>
  [[nodiscard]] std::optional<EntityType *> as() const;

  /// Get the id of this entity which can be safely stored in other entities
  /// @return id for this entity
  [[nodiscard]] EntityID get_entity_id() const;

protected:
  friend class GameState;

  /// Get pointer to the parent entity
  /// @note the pointer is not guarenteed to remain valid and should only be
  /// held for the duration of a function, EntityIDs should be used for storage
  /// @return non-null pointer to parent entity if successful, error if parent
  /// entity is of a different type or if the parent entity no longer exists
  template <
      typename EntityType,
      typename std::enable_if_t<std::is_base_of_v<Entity, EntityType>, int> = 0>
  [[nodiscard]] Result<EntityType *, std::string> get_parent_entity() const;

  /// Set the parent entity
  /// @param[in] parent_entity_id id of the parent entity
  void set_parent_entity(const EntityID parent_entity_id);

  /// Add a new entity as a child of this one and call's it's init function
  /// @note this assumes that the child entity can be constructed from a
  /// GameState reference
  /// @note the pointer is not guarenteed to remain valid and should only be
  /// held for the duration of a function, EntityIDs should be used for storage
  /// @tparam EntityType type of entity to add
  /// @tparam Args signature of entity's init function
  /// @param args arguements which are forwarded to the constructor the entity
  /// @return non-null pointer to new child entity if successful
  template <
      typename EntityType, typename... Args,
      typename std::enable_if_t<std::is_base_of_v<Entity, EntityType>, int> = 0>
  [[nodiscard]] Result<EntityType *, std::string>
  add_child_entity_and_init(Args &&...args);

  /// Add a new entity as a child of this one and call's it's init function
  /// @note this assumes that the child entity can be constructed from a
  /// GameState reference
  /// @note the pointer is not guarenteed to remain valid and should only be
  /// held for the duration of a function, EntityIDs should be used for storage
  /// @tparam EntityType type of entity to add
  /// @return non-null pointer to new child entity if successful
  template <
      typename EntityType,
      typename std::enable_if_t<std::is_base_of_v<Entity, EntityType>, int> = 0>
  [[nodiscard]] Result<EntityType *, std::string> add_child_entity();

  /// Remove entity from game state
  /// @note if entity_id is a child it will be removed from our child list
  /// @note if the entity_id is not in the game this function is a no-op
  /// @param[in] entity_id to remove
  void remove_entity(const EntityID entity_id);

  /// Remove all child entities
  /// @note this is called on destruction
  void remove_child_entities();

  /// Constructs and adds a new component of the speicified type to the entity
  /// @note components are owned by the entity
  /// @tparam ComponentType
  /// @tparam Args signature of components constructor
  /// @param args arguements which are forwarded to the constructor the
  /// ComponentType
  /// @return non-null pointer to new child entity if successful
  template <
      typename ComponentType, typename... Args,
      typename std::enable_if_t<
          std::is_base_of_v<component::Component, ComponentType>, int> = 0>
  component::Component *add_component(Args &&...args);

  /// underlying reference the game state note, that entities are owned by the
  /// game state so it will always outlive the entity
  GameState &game_state_;

  std::vector<std::unique_ptr<component::Component>> components_;

private:
  /// Attempt to get the parent entity as an untyped entity
  /// @return non-null entity pointer if successful nullopt otherwise
  [[nodiscard]] std::optional<Entity *> try_get_parent_entity() const;

  EntityID entity_id_;
  std::optional<EntityID> maybe_parent_entity_;
  std::vector<EntityID> child_entities_;
};

/// Class which holds all of the information about the current state of the game
class GameState {
public:
  GameState();

  /// Add a new entity to the game state
  /// @param[in] entity entity to be added to the game state
  [[nodiscard]] Result<EntityID, std::string>
  add_entity(std::unique_ptr<Entity> entity);

  template <typename SystemType,
            typename std::enable_if_t<
                std::is_base_of_v<systems::System, SystemType>, int> = 0>
  void add_system();

  void remove_entity(const EntityID id);

  /// Update the all of the entities in the game
  /// @param[in] delta_time_ns the current time in nanoseconds
  [[nodiscard]] Result<void, std::string>
  advance_state(const int64_t delta_time_ns);

  /// Handle mouse down event
  [[nodiscard]] Result<void, std::string>
  handle_event(const view::EventType &event, const view::Screen &screen);

  [[nodiscard]] std::optional<Entity *>
  try_get_entity_pointer_by_id(const EntityID entity_id) const;

  template <
      typename EntityType,
      typename std::enable_if_t<std::is_base_of_v<Entity, EntityType>, int> = 0>
  [[nodiscard]] Result<EntityType *, std::string>
  get_entity_pointer_by_id_as(const EntityID entity_id) const;

  /// TODO: add SFINAE or somthing
  template <
      typename EntityType,
      typename std::enable_if_t<std::is_base_of_v<Entity, EntityType>, int> = 0>
  [[nodiscard]] Result<EntityType *, std::string>
  get_entity_pointer_by_type() const;

  template <
      typename EntityType,
      typename std::enable_if_t<std::is_base_of_v<Entity, EntityType>, int> = 0>
  [[nodiscard]] std::vector<EntityType *> get_entity_pointers_by_type() const;

  template <
      typename EntityType,
      typename std::enable_if_t<std::is_base_of_v<Entity, EntityType>, int> = 0>
  void remove_entities_by_type();

  template <typename ComponentType,
            typename std::enable_if_t<
                std::is_base_of_v<component::Component, ComponentType>, int>>
  [[nodiscard]] std::vector<Entity *> get_entities_with_component() const;

  [[nodiscard]] Result<void, std::string> draw(view::Screen &screen) const;

private:
  static constexpr std::size_t max_entity_count{4096UL};
  static constexpr EntityID invalid_entity_id{
      std::numeric_limits<EntityID>::max()};

  [[nodiscard]] Result<bool, std::string>
  handle_mouse_up_for_entity(Entity &entity, const view::MouseUpEvent &event,
                             const view::Screen &screen);
  /// Current entities in the game
  std::vector<std::unique_ptr<Entity>> entities_;

  std::vector<std::unique_ptr<systems::System>> systems_;
};
} // namespace model

#include "model/game_state.inl"
