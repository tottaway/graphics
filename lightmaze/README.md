# LightMaze - 2D Platformer Puzzle Game

## Game Overview

LightMaze is a 2D side-scrolling platformer puzzle game featuring innovative lighting-based mechanics. Players control a character who can emit different colored lights, which fundamentally changes how they interact with the game world. Objects only provide collision when they appear different from the background under the current light color, creating unique puzzle opportunities.

## Core Mechanics

### Lighting System
- **Dynamic Light Source**: Player emits a circular light that can change colors
- **Color-Based Collision**: Objects only collide with the player when they appear visually distinct from the background under current lighting
- **Example**: Red wall on white background becomes invisible (non-solid) under pure red light, allowing passage

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

**Lighting Components:**
- `component::LightEmitter` - Generic light source with configurable color, radius, and intensity
- `component::LightReceiver` - Generic component for objects that react to lighting
- `systems::LightingSystem` - Generic lighting calculation and rendering system

### LightMaze-Specific Components

**Game Logic:**
- `lightmaze::ColorCollision` - Game-specific collision behavior based on color visibility
- `lightmaze::ColorInventory` - Player's collected light colors
- `lightmaze::ColorPickup` - Collectible color items
- `lightmaze::ColorGate` - Areas unlocked by specific colors

**Map System:**
- `lightmaze::PlatformTile` - Platform objects with color properties
- `lightmaze::MapEditor` - In-game level editing interface
- `lightmaze::MapSerializer` - Save/load map data to disk

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

### Phase 3: Enhanced Level Design Tools
- [ ] Create additional static platform entities
- [ ] Add level boundaries and camera constraints
- [ ] Implement basic level transitions
- [ ] Create simple level editor for rapid prototyping
- [ ] Design test levels focused on platformer mechanics

### Phase 4: Generic Lighting System (Add to components/ and systems/ folders)
- [ ] Implement `component::LightEmitter` as generic light source
- [ ] Create `component::LightReceiver` for objects affected by light
- [ ] Develop `systems::LightingSystem` for light calculation and rendering
- [ ] Add basic circular light emission and color management

### Phase 5: Game-Specific Lighting Mechanics (LightMaze folder)
- [ ] Implement `lightmaze::ColorCollision` using generic lighting data
- [ ] Create color-based visibility and collision logic
- [ ] Add player color switching mechanics
- [ ] Test lighting-based collision interactions

### Phase 6: Map System and Content (LightMaze folder)
- [ ] Design platform-based tile system using existing collision framework
- [ ] Create hand-crafted levels showcasing lighting mechanics
- [ ] Implement `lightmaze::ColorInventory` progression system
- [ ] Create `lightmaze::ColorPickup` collectibles
- [ ] Add `lightmaze::ColorGate` unlock mechanics

### Phase 7: Map Editor and Serialization
- [ ] Build `lightmaze::MapEditor` component for in-game level editing
- [ ] Implement tile placement using existing input system
- [ ] Add color assignment for map objects
- [ ] Create `lightmaze::MapSerializer` for save/load functionality

### Phase 8: Polish & Testing
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