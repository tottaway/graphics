# View Module

The view module handles all rendering, windowing, and input management for the game engine using SFML as the underlying graphics library.

## Core Classes

### Screen (`screen.hh`)

The main rendering and window management class that provides the interface between the game engine and SFML.

**Key functionality:**
- Window creation and management
- Coordinate system transformations (game world ↔ screen pixels)
- Drawing primitives (rectangles, textures, text)
- Event polling and processing
- Viewport management with configurable center and size

**Usage:**
```cpp
// Create screen with 2x2 meter viewport
auto screen = std::make_unique<view::Screen>(
    Eigen::Vector2f{2.0f, 2.0f},  // viewport size in meters
    Eigen::Vector2f{0.0f, 0.0f}   // viewport center
);

// Drawing operations
screen->draw_rectangle(bottom_left, top_right, color, z_level);
screen->draw_rectangle(bottom_left, top_right, texture, z_level);
screen->draw_text(location, font_size, "Hello World", color);

// Viewport control
screen->set_viewport_center(new_center);

// Event handling
bool should_continue = screen->poll_events_and_check_for_close().unwrap();
for (const auto &event : screen->get_events()) {
    // Process events
}
screen->clear_events();
```

**Coordinate Systems:**
- **Game meters**: World coordinates used by entities
- **Viewport meters**: Centered coordinate system for rendering
- **Viewport pixels**: Screen pixels relative to viewport center
- **Window pixels**: Absolute screen pixel coordinates

### Texture (`texture.hh`)

Manages texture loading, caching, and UV coordinate mapping for sprite rendering.

**Key features:**
- Automatic texture caching (same file loaded once)
- UV coordinate support for sprite sheets
- SFML texture wrapping with shared ownership

**Usage:**
```cpp
// Load full texture
view::Texture full_texture("path/to/image.png");

// Load texture region (for sprite sheets)
view::Texture sprite_texture("path/to/spritesheet.png",
                             Eigen::Vector2i{0, 0},      // bottom-left
                             Eigen::Vector2i{32, 32});   // top-right
```

### TextureSet (`tileset/texture_set.cc`)

Advanced texture management system for organizing sprite sheets using YAML configuration files.

**Features:**
- YAML-based sprite sheet configuration
- Automatic sprite extraction with padding support
- Named texture subsections
- Horizontal/vertical tile counting
- Coordinate system reflection support

**YAML Configuration Example:**
```yaml
- file_name: "character_sprites.png"
  subsections:
    idle:
      start: [0, 0]
      tile_size: [32, 32]
      padding: [1, 1]
      horizontal_tile_count: 4
      vertical_tile_count: 1
      reflect_x_axis: false
    walk:
      start: [0, 32]
      tile_size: [32, 32]
      padding: [1, 1]
      horizontal_tile_count: 8
      vertical_tile_count: 1
```

**Usage:**
```cpp
auto* texture_set = view::TextureSet::parse_texture_set("sprites/character.yaml").unwrap();
auto idle_textures = texture_set->get_texture_set_by_name("idle");
auto walk_textures = texture_set->get_texture_set_by_name("walk");
```

## Event System

### Event Types

```cpp
enum class MouseButton { Left, Right, Middle, XButton1, XButton2 };

struct MouseDownEvent {
    MouseButton button;
    Eigen::Vector2f position;
};

struct MouseUpEvent {
    MouseButton button;
    Eigen::Vector2f position;
};

struct MouseMovedEvent {
    Eigen::Vector2f position;
};

struct KeyPressedEvent {
    sf::Event::KeyEvent key_event;
};

struct KeyReleasedEvent {
    sf::Event::KeyEvent key_event;
};

using EventType = std::variant<MouseUpEvent, MouseMovedEvent, MouseDownEvent,
                               KeyPressedEvent, KeyReleasedEvent>;
```

### Event Processing Pattern

```cpp
// In game loop
if (!screen->poll_events_and_check_for_close().unwrap()) {
    return;  // Window was closed
}

for (const auto &event : screen->get_events()) {
    std::visit([](const auto &e) {
        // Handle specific event type
    }, event);
}
screen->clear_events();
```

## Rendering Pipeline

### Frame Structure
```cpp
screen->start_update();
// ... draw all entities
screen->finish_update();
```

### Z-Level Ordering
- Lower z-levels are drawn first (background)
- Higher z-levels are drawn last (foreground)
- Entities control their drawing order through `get_z_level()`

## Color System

```cpp
struct Color {
    int r, g, b;  // RGB values 0-255
};

// Common colors
view::Color white{255, 255, 255};
view::Color red{255, 0, 0};
view::Color transparent{0, 0, 0};  // Note: no alpha channel in basic Color
```

## Integration Example

See `view/main.cc` and the wiz game for complete integration examples:

```cpp
// Initialize graphics system
auto screen = std::make_unique<view::Screen>();

// Load game assets
auto texture_set = view::TextureSet::parse_texture_set("assets/sprites.yaml");

// In entity draw() method
Result<void, std::string> MyEntity::draw(view::Screen &screen) const {
    screen.draw_rectangle(get_bottom_left(), get_top_right(), my_texture, z_level);
    return Ok();
}
```

## Dependencies

- **SFML**: Core graphics, windowing, and input
- **ImGui**: Optional UI rendering support
- **yaml-cpp**: TextureSet configuration parsing
- **Eigen**: Vector and matrix operations

## Performance Notes

- Textures are cached automatically to avoid duplicate loading
- UV coordinates enable efficient sprite sheet usage
- Coordinate transformations are computed once per frame
- Event polling is efficient and non-blocking