# Model Module

The model module contains the core game state management and entity system for the game engine.

## Core Classes

### GameState (`game_state.hh`)

The main class that manages the entire game state and coordinates all entities.

**Key functionality:**
- Entity lifecycle management (creation, update, removal)
- Event handling and distribution to entities
- System execution coordination
- Tells view classes what to draw through entity draw() calls
- Mouse and keyboard event routing

**Usage:**
```cpp
// Create game state
auto game_state = std::make_unique<model::GameState>();

// Add entities
auto entity_id = game_state->add_entity(std::make_unique<MyEntity>(*game_state));

// Game loop operations
game_state->advance_state(delta_time_ns);
game_state->handle_event(event, screen);
game_state->draw(screen);
```

### Entity (`game_state.hh`)

Base class for all game objects. Entities are the primary building blocks of the game world.

**Key features:**
- Component-based architecture
- Event handling (mouse, keyboard)
- Transform management
- Parent-child relationships
- Lifecycle hooks (update, late_update, should_remove)

**Usage:**
```cpp
class MyEntity : public model::Entity {
public:
    static constexpr std::string_view entity_type_name = "my_entity";

    MyEntity(model::GameState &game_state) : Entity(game_state) {}

    Result<void, std::string> init() {
        // Add components
        add_component<component::Sprite>(...);
        add_component<component::Collider>(...);
        return Ok();
    }

    std::string_view get_entity_type_name() const override {
        return entity_type_name;
    }

    Result<void, std::string> update(int64_t delta_time_ns) override {
        // Update entity logic
        return Ok();
    }
};
```

### EntityID (`entity_id.hh`)

Safe identifier for referencing entities across the system.

**Key features:**
- Index-based with epoch validation to prevent stale references
- Equality comparison support
- Safe for storage in other entities

**Usage:**
```cpp
EntityID player_id = player_entity->get_entity_id();
// Store safely in other entities
Entity* player = game_state.try_get_entity_pointer_by_id(player_id);
```

### StaticDrawnRectangle (`rectangle.hh`)

A simple entity that draws a colored rectangle at a fixed position.

**Usage:**
```cpp
auto rect = game_state.add_entity(std::make_unique<StaticDrawnRectangle>(game_state));
rect->init(transform, color);
```

## Example Implementation

See the `wiz` folder for comprehensive examples:
- `Player` entity with components and event handling
- `Map` entity managing child `GrassTile` entities
- Component usage patterns
- Parent-child entity relationships

## Architecture Notes

- Entities own their components and manage their lifecycles
- The GameState owns all entities and coordinates their execution
- Entity updates happen in phases: update() → systems → late_update()
- Z-level determines drawing order (lower values drawn first)
- Event handling supports propagation control (return false to stop propagation)

## Event Handling Return Values ⚠️ IMPORTANT

**CRITICAL**: Event handler return values control event propagation in a counter-intuitive way:

- **`return Ok(false)`** = "Event consumed/handled, STOP processing other entities"
- **`return Ok(true)`** = "Event not handled, CONTINUE processing other entities"

### Common Mistakes to Avoid:
```cpp
// ❌ WRONG - This allows multiple entities to handle the same key press
case sf::Keyboard::A:
    move_left_pressed_ = true;
    return Ok(true);  // BUG: Other entities will also receive this event

// ✅ CORRECT - This properly consumes the event
case sf::Keyboard::A:
    move_left_pressed_ = true;
    return Ok(false); // Event handled, stop processing other entities

// ✅ CORRECT - This passes through unhandled events
default:
    return Ok(true);  // Event not handled, let other entities try
```

### Event Processing Order:
1. GameState iterates through entities in array order
2. Each entity's event handler is called
3. If handler returns `false`, processing stops immediately
4. If handler returns `true`, processing continues to next entity

### Best Practices:
- Return `false` when your entity consumes/handles the event
- Return `true` when your entity doesn't handle the event
- Only handle events that are specifically relevant to your entity
- Use descriptive comments to clarify the return value intent