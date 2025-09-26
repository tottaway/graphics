# Systems Directory

This directory contains game systems that operate on entities and their components to provide core game functionality. All systems inherit from the base `System` class and are managed by the game state.

## System Architecture

### Base System Class (`system.hh`)
All systems inherit from `systems::System` which provides:
- `update(GameState&, delta_time_ns)` - Called every frame to update system logic
- `draw(Screen&)` - Optional rendering method called after entity rendering
- `get_system_type_name()` - Returns unique identifier for the system

Systems are added to `GameState` using `add_system<SystemType>()` and run automatically during the game loop.

## Implemented Systems

### Collision System (`collisions.hh/.cc`)
**Purpose**: Manages all collision detection and response between entities with collider components.

**Key Features**:
- QuadTree-based spatial partitioning for efficient collision detection
- Support for multiple collider types (SolidAABB, NonCollidableAABB, JumpReset)
- Configurable interaction types for different collision behaviors
- Translation callbacks for physics response
- Optimized for large numbers of entities

**Usage**:
- Entities add collider components (SolidAABBCollider, NonCollidableAABBCollider, etc.)
- System automatically detects and handles collisions between compatible types
- Custom interaction types can be defined for game-specific collision logic

### Lighting System (`lighting_system.hh/.cc`) ✅ NEW
**Purpose**: Manages dynamic lighting effects and renders lighting overlays using GLSL shaders.

**Key Features**:
- **Multi-Light Support**: Collects and renders multiple light sources simultaneously
- **Shader-Based Rendering**: Uses GLSL shaders for realistic lighting with smooth falloff
- **"Black by Default" Model**: Only illuminated areas are visible, rest is dark
- **Extensible Geometry**: Supports circular, global, and custom light shapes
- **Performance Optimized**: Batches all lights into single shader render pass

**Implementation Details**:
- Collects all `LightEmitter` components during `update()`
- Renders lighting overlay during `draw()` using custom shaders
- Supports configurable light colors, intensities, and geometries
- Z-level integration: black overlay at z=-1.0, lights at z=-0.5

**Shader Files**:
- `systems/assets/shaders/lighting.vert` - Vertex shader for fullscreen quad
- `systems/assets/shaders/lighting.frag` - Fragment shader for lighting calculations

**Usage**:
```cpp
// Add to game state
game_state.add_system<systems::LightingSystem>();

// Add light to entity
entity->add_component<component::LightEmitter>(
    component::LightEmitter::CircularLightParams{
        .transform_func = [this]() { return get_transform(); },
        .radius_meters = 1.0f,
        .color = {255, 255, 200},  // Warm white
        .intensity = 1.0f
    });
```

## System Integration

### Update Order
Systems are updated in the order they were added to `GameState`. Current typical order:
1. **Collision System** - Handles physics and collision detection first
2. **Lighting System** - Updates after physics for current entity positions

### Rendering Integration
- Entity rendering happens first (z-levels 0+)
- System `draw()` methods called after entity rendering
- Lighting system renders at negative z-levels (-1.0 to -0.5) to appear over entities

### Component Dependencies
- **Collision System**: Requires entities to have collider components
- **Lighting System**: Requires entities to have `LightEmitter` components
- Systems automatically skip entities without required components

## Adding New Systems

### Basic System Template
```cpp
// systems/my_system.hh
#pragma once
#include "systems/system.hh"

namespace systems {
class MySystem : public System {
public:
    MySystem() = default;

    [[nodiscard]] virtual Result<void, std::string>
    update(model::GameState& game_state, const int64_t delta_time_ns) override;

    [[nodiscard]] virtual Result<void, std::string>
    draw(view::Screen& screen) override;

    [[nodiscard]] virtual std::string_view get_system_type_name() const override {
        return "my_system";
    }
};
}
```

### Integration Steps
1. Create system class inheriting from `System`
2. Implement required virtual methods
3. Add to game state: `game_state.add_system<MySystem>()`
4. Create any required components for entities to use
5. Add Bazel build targets in `systems/BUILD.bazel`

## Best Practices

### Performance
- Use efficient data structures (QuadTree example in collision system)
- Batch operations when possible (lighting system shader approach)
- Skip inactive or irrelevant entities early
- Cache expensive calculations between frames

### Component Interaction
- Systems should operate on components, not modify entities directly
- Use component callbacks for entity state changes
- Respect component ownership (entities own components, systems use them)
- Use interaction types for flexible collision/system behaviors

### Error Handling
- Always return `Result<void, std::string>` for proper error propagation
- Use `TRY_VOID()` macro for chaining operations that may fail
- Provide descriptive error messages for debugging
- Gracefully handle missing components or invalid states

### Extensibility
- Design systems to be configurable through component parameters
- Use templates or inheritance for specialized system variants
- Separate rendering logic from game logic where possible
- Consider future needs when designing component interfaces