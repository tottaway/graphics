# Components Module

The components module provides a component-based architecture system where entities can be composed of multiple reusable behaviors and capabilities.

## Core Architecture

### Component (`component.hh`)

Base class for all components that can be attached to entities.

**Key features:**
- Lifecycle methods: `update()`, `late_update()`, `draw()`
- Type identification through `get_component_type_name()`
- Owned and managed by entities

## Drawing Components

### Sprite (`sprite.hh`)

Renders a texture at a specified transform with z-level support.

**Usage:**
```cpp
auto sprite = entity->add_component<component::Sprite>([this]() {
    return component::Sprite::SpriteInfo{
        .transform = get_transform(),
        .texture = my_texture,
        .z_level = 1.0f
    };
});
```

### Animation (`animation.hh`)

Extends Sprite to provide animated texture sequences with configurable frame rates.

**Usage:**
```cpp
auto animation = entity->add_component<component::Animation>(
    [this]() { return get_transform(); },
    texture_vector,
    10.0f  // fps
);
```

### Label (`label.hh`)

Renders text with customizable font, color, and positioning.

**Usage:**
```cpp
auto label = entity->add_component<component::Label>([this]() {
    return component::Label::TextInfo{
        .text = "Hello World",
        .color = {255, 255, 255},
        .font = 16.0f,
        .transform = get_transform()
    };
});
```

### DrawRectangle (`draw_rectangle.hh`)

Renders colored rectangles (useful for debugging or simple shapes).

### DrawGridCell (`draw_grid_cell.hh`)

Specialized component for rendering grid-based layouts.

## Collision Components

### Collider (`collider.hh`)

Base class for all collision detection components with a sophisticated interaction system.

**Key features:**
- Multiple collider types: `static_object`, `non_collidable`, `solid`
- Interaction type system with bitmask filtering
- Callback-based collision handling
- AABB (Axis-Aligned Bounding Box) shape support

**Interaction Types:**
- `hit_box_collider` / `hurt_box_collider` - Damage system
- `wiz_good_*` / `wiz_bad_*` / `wiz_neutral_*` - Faction-based collisions
- `solid_collider` - Physical blocking
- `wiz_grass_tile_collider` - Environment interactions

### SolidAABBCollider

Provides physical collision with movement blocking.

**Usage:**
```cpp
auto collider = entity->add_component<component::SolidAABBCollider>(
    [this]() { return get_transform(); },  // get transform
    [this](const Eigen::Vector2f &translation) {  // move callback
        this->position += translation;
    }
);
```

### NonCollidableAABBCollider

Provides collision detection without blocking movement (triggers).

**Usage:**
```cpp
auto trigger = entity->add_component<component::NonCollidableAABBCollider>(
    [this]() { return get_transform(); },
    [this](const model::EntityID other_id) {  // collision callback
        // Handle collision with other entity
    }
);
```

### HitBox / HurtBox (`hit_box.hh`, `hurt_box.hh`)

Specialized colliders for damage systems:
- **HitBox**: Can deal damage to HurtBoxes
- **HurtBox**: Can receive damage from HitBoxes

## Utility Components

### Center (`center.hh`)

Provides automatic centering functionality for entities.

### GridCollider (`grid_collider.hh`)

Specialized collider for grid-based collision detection.

## Usage Patterns

### Basic Component Addition
```cpp
// In entity's init() method
add_component<ComponentType>(constructor_args...);
```

### Component Retrieval
```cpp
// Get single component
auto* sprite = entity->get_component<component::Sprite>();

// Get all components of type
auto sprites = entity->get_components<component::Sprite>();

// Get all components
auto all_components = entity->get_components();
```

### Component Removal
```cpp
entity->remove_components<component::Sprite>();
```

## Example Implementation

See `wiz/player.cc` for comprehensive component usage:

```cpp
Result<void, std::string> Player::init() {
    // Centering
    add_component<component::Center>([this]() { return get_transform(); });

    // Physics
    add_component<component::SolidAABBCollider>(
        [this]() { return get_hurt_box_transform(); },
        [this](const Eigen::Vector2f &translation) {
            this->position += translation;
        });

    // Combat system
    add_component<WizHurtBox<Alignement::good>>(
        [this]() { return get_hurt_box_transform(); },
        [this]() { was_hit_ = true; });

    add_component<WizHitBox<Alignement::good>>(
        [this]() { return get_hit_box_transform(); });

    // UI
    add_component<HealthBar>(hp, [this]() { return hp; },
                            [this]() { return get_transform(); });

    // Animation
    add_component<CharacterAnimationSet>(...);

    return Ok();
}
```

## Design Principles

- Components are owned by entities and destroyed with them
- Components communicate through the parent entity
- Use function callbacks for dynamic data (transforms, states)
- Collision system uses bitmasks for efficient filtering
- Drawing components use z-levels for layering