# Claude Session Notes

## Project Context
This appears to be a graphics project with a "wiz game" component that includes map generation functionality.

## Key Findings
- Successfully implemented cellular automata and room-corridor map generators
- Both generators use template parameters for map size and inline implementation files
- Initial crash was resolved by using conservative generation parameters (30% wall probability, 3 smoothing iterations)
- Added detailed error handling and debug output for future debugging
- Entity sizes: Player (0.1f x 0.1f = 1 tile), Skeleton (0.07f x 0.1f = 0.7x1 tiles)
- Added passage width validation to ensure 2+ tile wide passages for entity movement
- Complete connectivity solution with visual-functional consistency (391 walls, 509 grass tiles)
- 2x2 block-based connectivity detection identifies truly disconnected regions (found 3 components)
- Intelligent corridor creation connects isolated areas with minimal disruption (6 walls removed)
- Visual consistency validation ensures all walkable areas have adequate entity clearance (0 tiles needed filling)
- Perfect UX: what looks walkable IS walkable, no confusing narrow passages

## Useful Commands
- Run wiz game: `bazel run //wiz:wiz_main --enable_workspace=true` (builds automatically before running)
- Run lightmaze game: `bazel run //lightmaze:lightmaze_main --enable_workspace=true` (builds automatically before running)
- Run all tests: `bazel test //... --enable_workspace=true --test_output=all`
- Run specific test: `bazel test //lightmaze/tests:map_entity_test --enable_workspace=true --test_output=all`

## Architecture Notes
- Map configuration: 30x30 tiles, tile_size = 0.2f (doubled from original 0.1f), total map = 6x6 meters
- Camera viewport: 2.5x2.5 meters (increased from default 2x2 for better zoom level)
- Collision system bounds hardcoded in systems/collisions.cc: {7.0f, -1.0f, 7.0f, -1.0f} (updated to fix top-edge collisions)
- **SYNC ISSUE**: Collision bounds can go out of sync with map configuration - currently manually updated for doubled tile size
- QuadTree is private class in collision system, bounds not exposed to external systems
- **COORDINATE SYSTEM ODDITY**: A square with "size" 1 actually has vertices at (1,1), (-1,1), (-1,-1), (1,-1), meaning it has side lengths of 2. This is inherited from the engine design and difficult to change due to dependencies across multiple games. Platform creation logic must account for this by dividing drag distances by 2 to get actual platform sizes.

## Session History
- Session 1: Created project guideline files (GUIDELINES.md, CLAUDE.md)
- Session 2: Implemented comprehensive map generation system
  - Created two procedural algorithms: cellular automata and room-corridor
  - Added template-based generators with .inl implementation files
  - Fixed collision system bounds and camera viewport for better gameplay
  - Optimized room-corridor parameters for open spaces (6-12 tile rooms, 3x3 grid, 5% interior walls)
  - Doubled tile size and adjusted camera to 2.5x2.5m viewport for better visual scale
- Session 3: Implemented LightMaze platformer game
  - Created complete 2D platformer with player entity, platforms, and physics
  - Added Gravity component for realistic downward acceleration
  - Implemented Jumper component with double jump mechanics and coyote time prevention
  - Created JumpReset component for precise ground detection on platform tops
  - Fixed collision system interaction types and enhanced component type identification
  - Debugged and resolved multiple platformer physics issues (bouncing, input handling, collision detection)
  - Enhanced component documentation and update patterns
- Always make bazel targets when creating a new library, when importing a new library remember to add it as a bazel dep.

## LightMaze Game Details
- **Game Type**: 2D platformer with physics-based movement
- **Player Features**: Double jump mechanics with coyote time prevention
- **Physics**: Gravity component with realistic downward acceleration (-9.8 m/s²)
- **Collision System**: Enhanced with jump reset detection on platform tops
- **Components**: Gravity, Jumper (double jump), JumpReset (ground detection)
- **Key Mechanics**: Precise collision detection, bounce prevention, proper input handling

## Map Generation Details (Wiz Game)
- **Current algorithm**: Room-corridor (configurable via enum in map.hh)
- **Cellular automata**: Organic cave-like structures with 2x2 connectivity validation
- **Room-corridor**: Structured layouts with large open rooms connected by minimal corridors
- **Parameters tuned for gameplay**: Larger rooms (6-12 tiles), fewer interior walls (5%), better spawn probability (80%)
- **Debug features**: ASCII visualization, entity count reporting, full connectivity guaranteed

## LightMaze Map Editor Implementation Details (Session 4)
- **Enhanced Platform Manipulation**: Added right-click drag and double-click deletion to MapEntity class
- **Key Design Decisions**:
  - Platform entities handle their own mouse events rather than delegating to Map entity for simpler hit detection
  - Platforms track relative mouse movement (delta) rather than snapping to absolute mouse position for natural dragging feel
  - Entity bounds automatically handle hit detection - no need for manual contains_point checks
  - Platforms delete themselves using remove_entity(get_entity_id()) rather than asking parent to delete them
  - Double-click threshold set to 400ms for responsive feel
- **Architecture Improvements**:
  - Added SFML abstraction guideline - no direct SFML usage in game code
  - Added variant safety guideline - never blindly unwrap with std::get, always check with std::holds_alternative first
  - Enhanced session initialization workflow to include key header file reading
- **Implementation Details**:
  - Right-click starts drag mode, stores initial mouse position in drag_offset_
  - Mouse movement calculates delta from previous position and applies to platform
  - Double-click detection uses chrono::steady_clock with 400ms threshold
  - All unsafe std::get calls replaced with proper variant safety checks

### Session 4: Basic Lighting System Implementation
- **Lighting Architecture**: Implemented complete lighting system with LightEmitter component and LightingSystem
- **Component Design**: Created extensible LightEmitter with CircularLightGeometry and support for multiple light shapes
- **Rendering Pipeline**: Fixed OpenGL depth testing to support proper z-level ordering for lighting effects
- **Player Lighting**: Added 0.7 meter radius warm white light (255,255,200) that follows the player
- **Visual Effect**: Achieved black-by-default world with illuminated areas around light sources
- **Key Technical Solutions**:
  - Enabled OpenGL depth testing (GL_DEPTH_TEST, GL_LEQUAL) and depth buffer clearing
  - Used negative z-levels for lighting system (black overlay at z=-1.0, lights at z=-0.5) to appear in front of game entities
  - Systems draw() methods called after entity drawing, allowing proper layering
  - Parameter struct pattern for extensible component constructors (CircularLightParams, etc.)
- **Architecture Notes**:
  - LightingSystem collects lights in update(), renders black overlay + lights in draw()
  - LightEmitter component with geometry-based design ready for cones, rectangles, etc.
  - Z-level system: game entities (z=0,1) < black overlay (z=-1.0) < lights (z=-0.5)
  - Added visual bug fixing guideline: always prompt for user feedback after each attempt