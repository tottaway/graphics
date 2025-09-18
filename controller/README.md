# Controller Module

The controller module provides the main game loop coordination between the model and view systems.

## Core Classes

### Controller (`controller.hh`)

The central orchestrator that manages the game's main loop and coordinates between the model (GameState) and view (Screen) systems.

**Key functionality:**
- Main game loop execution with configurable update intervals
- Event polling and forwarding to game state
- Frame timing management and display
- Coordinated update sequence: events → game state → drawing

**Constructor:**
```cpp
Controller(std::unique_ptr<view::Screen> screen,
           std::unique_ptr<model::GameState> game_state);
```

**Usage:**
```cpp
// Create screen and game state
auto screen = std::make_unique<view::Screen>();
auto game_state = std::make_unique<model::GameState>();

// Create controller
controller::Controller controller(std::move(screen), std::move(game_state));

// Run the game loop
auto result = controller.run();  // Run with default timing
// OR
auto result = controller.run(std::chrono::milliseconds(16));  // ~60 FPS
```

## Game Loop Architecture

The controller implements a standard game loop pattern:

1. **Event Processing**: Poll window events and check for close requests
2. **Event Distribution**: Forward events to the game state for entity handling
3. **Game State Update**: Advance game state by delta time
4. **Rendering**: Trigger drawing of all entities through the screen

## Timing Control

- **Default Mode**: Updates as fast as possible
- **Throttled Mode**: Respects minimum update interval for consistent frame rates
- Frame duration is logged to console for performance monitoring

## Error Handling

The controller uses the Result pattern for error propagation:
- Returns `Ok()` when the game loop exits normally (window closed)
- Returns `Err(message)` if any subsystem encounters an error

## Example Integration

See `wiz/wiz_main.cc` for a complete example:

```cpp
int main() {
  auto game_result = wiz::make_wiz_game();
  if (game_result.isErr()) {
    std::cerr << "Failed to initialize game: " << game_result.unwrapErr() << std::endl;
    return EXIT_FAILURE;
  }

  controller::Controller controller(std::make_unique<view::Screen>(),
                                    std::move(game_result).unwrap());
  const auto result = controller.run();
  if (result.isErr()) {
    std::cerr << "Application ended with error: " << result.unwrapErr() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
```

## Design Notes

- The controller owns both the screen and game state, ensuring proper lifetime management
- Frame timing is handled at the controller level, not in individual entities
- The update interval is optional - games can run uncapped or with frame rate limiting