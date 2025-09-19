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
- Build wiz game: `bazel build //wiz:wiz_main --enable_workspace=true`
- Run wiz game: `bazel run //wiz:wiz_main --enable_workspace=true` (builds automatically before running)

## Architecture Notes
- Map configuration: 30x30 tiles, tile_size = 0.2f (doubled from original 0.1f), total map = 6x6 meters
- Camera viewport: 2.5x2.5 meters (increased from default 2x2 for better zoom level)
- Collision system bounds hardcoded in systems/collisions.cc: {7.0f, -1.0f, 7.0f, -1.0f} (updated to fix top-edge collisions)
- **SYNC ISSUE**: Collision bounds can go out of sync with map configuration - currently manually updated for doubled tile size
- QuadTree is private class in collision system, bounds not exposed to external systems

## Session History
- Session 1: Created project guideline files (GUIDELINES.md, CLAUDE.md)
- Session 2: Implemented comprehensive map generation system
  - Created two procedural algorithms: cellular automata and room-corridor
  - Added template-based generators with .inl implementation files
  - Fixed collision system bounds and camera viewport for better gameplay
  - Optimized room-corridor parameters for open spaces (6-12 tile rooms, 3x3 grid, 5% interior walls)
  - Doubled tile size and adjusted camera to 2.5x2.5m viewport for better visual scale
- Always make bazel targets when creating a new library, when importing a new library remember to add it as a bazel dep.

## Map Generation Details
- **Current algorithm**: Room-corridor (configurable via enum in map.hh)
- **Cellular automata**: Organic cave-like structures with 2x2 connectivity validation
- **Room-corridor**: Structured layouts with large open rooms connected by minimal corridors
- **Parameters tuned for gameplay**: Larger rooms (6-12 tiles), fewer interior walls (5%), better spawn probability (80%)
- **Debug features**: ASCII visualization, entity count reporting, full connectivity guaranteed