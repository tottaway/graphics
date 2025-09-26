# LightMaze - 2D Platformer Puzzle Game

## Game Overview

LightMaze is a 2D side-scrolling platformer puzzle game featuring innovative lighting-based mechanics. Players control a character who can emit different colored lights, which fundamentally changes how they interact with the game world. Objects only provide collision when they appear different from the background under the current light color, creating unique puzzle opportunities.

## Core Mechanics

### Lighting System ✅ IMPLEMENTED
- **Dynamic Light Source**: Player emits a circular light that can change colors in real-time
- **Color-Based Collision**: Platforms become non-solid when illuminated by light matching their color
- **Real-time Switching**: Press keys 1-4 to change light color (White, Red, Blue, Green)
- **Visual Feedback**: Lighting system provides immediate visual feedback showing which platforms are affected
- **Physics Integration**: Dynamic collision components are added/removed based on lighting state each frame

### Controls
- **Movement**: Arrow keys or WASD for horizontal movement
- **Jump**: Space bar or Up arrow (supports double jump)
- **Light Colors**: Number keys 1-4 to switch light colors
  - 1: White light (illuminates all platforms)
  - 2: Red light (makes red platforms non-solid)
  - 3: Blue light (makes blue platforms non-solid)
  - 4: Green light (makes green platforms non-solid)
- **Editor Mode**: E key to toggle between gameplay and map editor
- **Platform Creation**: Left-click drag in editor mode
- **Platform Movement**: Right-click drag platforms in editor mode
- **Platform Deletion**: Double right-click platforms in editor mode
- **Platform Colors**: Number keys 1-4 while dragging to set platform colors

### Metroidvania Progression
- **Color Collection**: Discover new light colors throughout the world
- **Area Unlocking**: New colors grant access to previously inaccessible regions
- **Non-linear Exploration**: Interconnected world with optional paths and secrets

### Platformer Physics
- **Gravity System**: Standard downward gravity affects player movement
- **Jump Mechanics**: Variable height jumping with momentum conservation
- **Platform Navigation**: Traditional side-scrolling movement with precise collision detection

## Technical Architecture

### Reusable Components (from existing engine)
The following components from the main engine will be reused:

**Core Framework:**
- `model::GameState` - Entity lifecycle and event management
- `model::Entity` - Base entity class with component system
- `controller::Controller` - Main game loop coordination
- `view::Screen` - Rendering, windowing, and input management

**Rendering Components:**
- `component::Sprite` - Basic texture rendering with z-levels
- `component::Animation` - Animated sprite sequences
- `component::Label` - Text rendering for UI elements
- `component::DrawRectangle` - Colored shapes for debugging

**Physics Components:**
- `component::SolidAABBCollider` - Physical collision with movement blocking
- `component::NonCollidableAABBCollider` - Trigger-based collision detection
- `systems::collisions` - QuadTree-based collision detection system

**Utilities:**
- `component::Center` - Automatic entity centering
- `view::Texture` - Texture loading and caching
- `view::TextureSet` - YAML-based sprite sheet management

### New Generic Components (for components/ folder)

**Physics Components:**
- `component::Gravity` - Generic downward acceleration component (reusable for any game with gravity)
- `component::Velocity` - Generic velocity tracking and momentum component
- `component::PlatformerMovement` - Jump mechanics and horizontal movement patterns

**Lighting Components:** ✅ IMPLEMENTED
- `component::LightEmitter` - Generic light source with configurable color, radius, and intensity
- `component::LightGeometry` - Extensible geometry system (circular, global light shapes)
- `systems::LightingSystem` - Generic lighting calculation and rendering system

### LightMaze-Specific Components

**Game Logic:** ✅ IMPLEMENTED
- `lightmaze::LightMazeCollider` - Platform collision detection that responds to matching light colors
- `lightmaze::LightMazeLightVolume` - Player light collision detection for color-based mechanics

**Map System:** ✅ IMPLEMENTED
- `lightmaze::MapEntity` - Platform objects with color properties and editor manipulation
- `lightmaze::MapModeManager` - In-game level editing interface with mode switching
- YAML serialization system for save/load map data

## Implementation Plan

### Phase 1: Core Framework ✅ COMPLETED
- [x] Set up basic game structure using existing `controller::Controller`
- [x] Implement basic entity system using `model::GameState` and `model::Entity`
- [x] Create player entity with basic movement using existing colliders
- [x] Set up basic sprite rendering and animation using `component::Animation`
- [x] Add static floor for visual reference and collision

### Phase 2: Refine Platformer Mechanics ✅ COMPLETED
- [x] Extract gravity logic into generic `component::Gravity` component
- [x] Improve ground detection using proper collision callbacks with `component::JumpReset`
- [x] Add coyote time prevention for better feel (prevents mid-air jumps after walking off platforms)
- [x] Implement double jump mechanics with proper state tracking
- [x] Create `component::Jumper` for jump management and collision state detection
- [x] Add platform collision with proper top-surface detection
- [x] Fix bounce prevention using grounded state detection
- [x] Test and polish basic platformer movement feel
- [ ] Create `component::Velocity` for momentum tracking and physics consistency
- [ ] Implement variable jump height based on input duration
- [ ] Add horizontal acceleration/deceleration curves
- [ ] Create multiple platform types (solid, one-way, moving)

### Phase 3: Enhanced Level Design Tools 🔄 IN PROGRESS
- [ ] **Map Editor Mode Implementation** - Primary focus for current development
- [ ] **Map Serialization System** - JSON-based save/load for platforms and player state
- [ ] **Auto-save Functionality** - Automatic saving every 5 seconds during editor mode
- [ ] Create additional static platform entities
- [ ] Add level boundaries and camera constraints
- [ ] Implement basic level transitions
- [ ] Design test levels focused on platformer mechanics

## Map Editor System Implementation Plan

### Overview
The map editor system allows real-time level editing while the game is running. Players can seamlessly switch between gameplay mode and editor mode, creating and modifying platforms while maintaining full player control.

### Core Features
- **Toggle Editor Mode**: Press 'E' key to enter/exit map editor mode
- **Platform Creation**: Left-click and drag to create new rectangular platforms
- **Platform Movement**: Right-click and drag existing platforms to reposition them
- **Platform Deletion**: Double right-click on platforms to remove them
- **Auto-save**: Game state automatically saves every 5 seconds
- **Persistent State**: Player position and all platforms saved/loaded on game start
- **Concurrent Play/Edit**: Player movement continues to work while in editor mode

### Revised Architecture Design

#### 1. Map Entity (`lightmaze/map.hh/.cc`)
Central entity that manages all platforms as children and handles editor mode:

```cpp
class Map : public model::Entity {
public:
    enum class EditorMode { gameplay, editor };

    Map(model::GameState& game_state);

    // Editor mode management
    bool is_editor_mode() const { return current_mode_ == EditorMode::editor; }
    Result<void, std::string> toggle_editor_mode();

    // Platform management
    Result<model::EntityID, std::string> add_platform(
        const Eigen::Vector2f& top_center_position,
        const Eigen::Vector2f& size,
        uint32_t platform_id = 0
    );
    Result<void, std::string> remove_platform(const model::EntityID& platform_id);

    // Save/load integration
    Result<void, std::string> save_current_state();
    Result<void, std::string> load_saved_state();

    // Event handling for editor operations
    Result<bool, std::string> on_key_press(const view::KeyPressedEvent& event) override;
    Result<bool, std::string> on_mouse_down(const view::MouseDownEvent& event) override;
    Result<bool, std::string> on_mouse_up(const view::MouseUpEvent& event) override;
    Result<bool, std::string> on_mouse_moved(const view::MouseMovedEvent& event) override;

    Result<void, std::string> update(const int64_t delta_time_ns) override;

private:
    EditorMode current_mode_{EditorMode::gameplay};

    // Platform creation state
    bool is_creating_platform_{false};
    Eigen::Vector2f creation_start_pos_;
    std::unique_ptr<Platform> preview_platform_;

    // Auto-save timer
    int64_t time_since_last_save_ns_{0};
    static constexpr int64_t auto_save_interval_ns_{5'000'000'000};  // 5 seconds

    // Platform ID management
    uint32_t next_platform_id_{1};

    // Helper methods
    Result<void, std::string> create_platform(const Eigen::Vector2f& start, const Eigen::Vector2f& end);
    Result<void, std::string> auto_save_if_needed();
};
```

#### 2. Enhanced Platform Class
Platforms handle their own right-click operations but check with parent Map:

```cpp
class Platform : public model::Entity {
public:
    Platform(model::GameState& game_state,
             const Eigen::Vector2f& top_center_position,
             const Eigen::Vector2f& size,
             uint32_t platform_id = 0);

    // Editor support methods
    uint32_t get_platform_id() const { return platform_id_; }
    void set_position(const Eigen::Vector2f& new_top_center_position);
    bool contains_point(const Eigen::Vector2f& world_point) const;

    // Event handling for right-click operations
    Result<bool, std::string> on_mouse_down(const view::MouseDownEvent& event) override;
    Result<bool, std::string> on_mouse_up(const view::MouseUpEvent& event) override;
    Result<bool, std::string> on_mouse_moved(const view::MouseMovedEvent& event) override;

    Result<void, std::string> update(const int64_t delta_time_ns) override;

private:
    uint32_t platform_id_;

    // Movement state
    bool is_being_moved_{false};
    Eigen::Vector2f drag_offset_;

    // Double-click detection for deletion
    std::chrono::steady_clock::time_point last_right_click_time_ns_;
    static constexpr int64_t double_click_threshold_ns_{500'000'000};  // 500ms

    // Helper methods
    Map* get_parent_map();  // Cast parent to Map entity
    Result<void, std::string> handle_right_click();
    Result<void, std::string> handle_double_right_click();
};
```

#### 3. Map Serialization System (`lightmaze/map_serializer.hh/.cc`)
YAML-based serialization leveraging existing yaml-cpp support:

```cpp
struct PlatformData {
    Eigen::Vector2f top_center_position;
    Eigen::Vector2f size;
    uint32_t platform_id;
};

struct GameStateData {
    Eigen::Vector2f player_position;
    std::vector<PlatformData> platforms;
    uint32_t next_platform_id;
};

class MapSerializer {
public:
    static Result<void, std::string> save_game_state(
        const GameStateData& state,
        const std::string& file_path = "lightmaze/saves/current_level.yaml"
    );

    static Result<GameStateData, std::string> load_game_state(
        const std::string& file_path = "lightmaze/saves/current_level.yaml"
    );

    static GameStateData extract_game_state(const model::GameState& game_state);
    static Result<void, std::string> apply_game_state(
        model::GameState& game_state,
        const GameStateData& state
    );
};
```

#### 4. Enhanced Mode Manager Integration
Update `LightMazeModeManager` to use Map entity instead of individual platforms:

```cpp
class LightMazeModeManager : public model::Entity {
private:
    Result<void, std::string> start_new_game() override;
    // Creates Map entity, which handles platform creation and save/load

    model::EntityID map_entity_id_;
};
```

### Catch2 Testing Framework Integration

#### Bazel Integration
Add to root `WORKSPACE` file:
```python
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "catch2",
    urls = ["https://github.com/catchorg/Catch2/archive/v3.4.0.tar.gz"],
    strip_prefix = "Catch2-3.4.0",
    sha256 = "122928b814b75717316c71af69bd2b43387643ba076a6ec16e7882bfb2dfacbb"
)
```

#### Test Structure
```
lightmaze/
├── tests/
│   ├── BUILD.bazel              # Test targets
│   ├── test_main.cc            # Catch2 main runner
│   ├── test_map_serializer.cc  # Serialization tests
│   ├── test_map_editor.cc      # Editor functionality tests
│   └── test_platform.cc       # Platform behavior tests
```

#### Sample Test Configuration (`lightmaze/tests/BUILD.bazel`)
```python
load("@rules_cc//cc:defs.bzl", "cc_test", "cc_library")

cc_library(
    name = "test_main",
    srcs = ["test_main.cc"],
    deps = ["@catch2//:catch2_main"],
)

cc_test(
    name = "map_serializer_test",
    srcs = ["test_map_serializer.cc"],
    deps = [
        ":test_main",
        "//lightmaze:map_serializer",
        "@catch2//:catch2",
    ],
)
```

### Implementation Steps

#### Step 1: Catch2 Framework Setup
1. Add Catch2 to WORKSPACE and configure Bazel integration
2. Create basic test structure with main runner
3. Implement simple smoke test to verify framework works
4. Add test build targets to lightmaze BUILD.bazel

#### Step 2: Map Entity Foundation
1. Create Map entity class with basic editor mode toggle (E key)
2. Integrate Map entity into LightMazeModeManager
3. Move platform creation logic from hardcoded to Map entity
4. Test basic Map entity functionality and editor mode switching

#### Step 3: MapSerializer with YAML
1. Implement MapSerializer class using existing yaml-cpp dependency
2. Create save file directory structure
3. Add unit tests for serialization round-trip accuracy
4. Test with current platform setup

#### Step 4: Enhanced Platform Class
1. Add platform ID system and editor support methods
2. Implement right-click event handling in Platform entities
3. Add platform movement and double-click deletion logic
4. Test platform manipulation with parent Map coordination

#### Step 5: Platform Creation in Map Entity
1. Implement left-click drag platform creation in Map
2. Add visual preview during creation
3. Integrate with platform ID management
4. Test platform creation workflow

#### Step 6: Auto-save and Polish
1. Implement auto-save timer in Map entity
2. Integrate save/load with game startup
3. Add comprehensive test coverage for all editor operations
4. Polish user experience with visual feedback

### YAML Save Format
```yaml
format_version: "1.0"
timestamp: "2025-01-20T10:30:00Z"
player_position:
  x: 0.5
  y: 1.2
platforms:
  - id: 1
    top_center_position:
      x: 0.0
      y: 0.0
    size:
      x: 1.0
      y: 0.2
  - id: 2
    top_center_position:
      x: 1.5
      y: 0.8
    size:
      x: 1.0
      y: 0.2
next_platform_id: 3
```

### File Structure Updates
```
lightmaze/
├── map.hh/.cc                  # New: Map entity (parent of platforms)
├── map_serializer.hh/.cc       # New: YAML save/load functionality
├── saves/                      # New: Save file directory
│   └── current_level.yaml      # Auto-generated save file
├── tests/                      # New: Catch2 test suite
│   ├── BUILD.bazel             # Test targets
│   ├── test_main.cc            # Test runner
│   └── test_*.cc               # Individual test files
├── platform.hh/.cc            # Modified: Add editor support and right-click handling
└── mode_manager.hh/.cc         # Modified: Use Map entity instead of direct platforms
```

### Key Benefits of This Architecture
1. **Clear Separation**: Map handles creation/E key, Platforms handle right-click operations
2. **Parent-Child Relationship**: Natural entity hierarchy with Map as platform container
3. **Concurrent Editing**: Player movement unaffected by editor mode
4. **YAML Integration**: Leverages existing project dependencies
5. **Test Coverage**: Catch2 framework for reliable testing
6. **Maintainable**: Simple, focused responsibilities for each entity

### Phase 4: Generic Lighting System ✅ COMPLETED
- [x] Implemented `component::LightEmitter` as generic light source with multiple geometries (circular, global)
- [x] Created extensible light geometry system supporting different light shapes and falloffs
- [x] Developed `systems::LightingSystem` for light calculation and rendering
- [x] Added comprehensive light configuration with color, intensity, and position management

### Phase 5: Game-Specific Lighting Mechanics ✅ COMPLETED
- [x] Implemented `lightmaze::LightMazeCollider` and `lightmaze::LightMazeLightVolume` components
- [x] Created dynamic color-based collision system where platforms become non-solid when illuminated by matching colors
- [x] Added player color switching mechanics (keys 1-4: White, Red, Blue, Green)
- [x] Integrated lighting collision detection with existing physics system

### Phase 6: Map System and Content (LightMaze folder)
- [ ] Design platform-based tile system using existing collision framework
- [ ] Create hand-crafted levels showcasing lighting mechanics
- [ ] Implement `lightmaze::ColorInventory` progression system
- [ ] Create `lightmaze::ColorPickup` collectibles
- [ ] Add `lightmaze::ColorGate` unlock mechanics

### Phase 7: Polish & Testing
- [ ] Optimize lighting shader performance
- [ ] Add UI elements using existing `component::Label`
- [ ] Create tutorial and hint systems
- [ ] Design interconnected world map
- [ ] Comprehensive playtesting and balancing

## File Structure

```
# Generic components (reusable across games)
components/
├── gravity.hh/.cc              # ✅ Generic gravity component
├── jumper.hh/.cc               # ✅ Generic jump mechanics with double jump and coyote time prevention
├── jump_reset.hh/.cc           # ✅ Generic ground detection component
├── velocity.hh/.cc             # Generic velocity/momentum tracking (planned)
├── light_emitter.hh/.cc        # Generic light source component (planned)
├── light_receiver.hh/.cc       # Generic light-affected objects (planned)
└── platformer_movement.hh/.cc  # Generic platformer physics (planned)

systems/
├── lighting_system.hh/.cc      # Generic lighting calculation system
└── ...

# Game-specific implementation
lightmaze/
├── README.md                   # This file
├── BUILD.bazel                # Bazel build configuration
├── lightmaze_main.cc          # Application entry point
├── lightmaze.hh/.cc           # Main game factory function
├── player.hh/.cc              # Player entity (game-specific)
├── mode_manager.hh/.cc        # Game mode management (game-specific)
├── components/                # LightMaze-specific components only
│   ├── color_collision.hh/.cc
│   ├── color_inventory.hh/.cc
│   ├── color_pickup.hh/.cc
│   └── color_gate.hh/.cc
├── systems/                   # LightMaze-specific systems
│   └── map_editor.hh/.cc
├── maps/                      # Level data files
│   ├── level_01.json
│   ├── level_02.json
│   └── ...
└── assets/                    # Game-specific assets
    ├── sprites/
    │   ├── player.yaml
    │   ├── tiles.yaml
    │   └── ...
    └── maps/
        └── ...
```

## Component Usage Strategy

### Generic Components Design
- `component::Gravity` - Configurable acceleration vector (not just downward)
- `component::LightEmitter` - Color, radius, intensity parameters for any lighting needs
- `component::Velocity` - Generic momentum tracking for any physics simulation
- `component::PlatformerMovement` - Reusable jump/walk mechanics for any platformer

### Game-Specific Extensions
- `lightmaze::ColorCollision` - Uses generic lighting data to determine collision behavior
- LightMaze entities use generic `component::Gravity` and `component::LightEmitter`
- Custom game logic builds on top of generic physics and lighting foundation

## Technical Considerations

### Reusability
- Generic gravity component can be used for top-down games with falling objects
- Generic lighting system supports any game needing dynamic lighting effects
- Platformer movement component usable for any side-scrolling game
- Lighting system designed for extension (spotlights, colored shadows, etc.)

### Performance
- Leverage existing QuadTree collision detection for efficiency
- Use existing texture caching system for optimal memory usage
- Implement color calculations in GPU shaders for 60fps performance
- Build on existing z-level rendering system for layered visuals

### Integration
- Follow existing component architecture patterns
- Use same Result<> error handling as existing codebase
- Maintain compatibility with existing event system
- Reuse existing coordinate transformation systems
