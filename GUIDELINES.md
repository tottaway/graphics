# Project Guidelines

## Best Practices for Working on This Project

### File Organization
- **GUIDELINES.md**: Contains project best practices and conventions (this file)
- **CLAUDE.md**: External scratch pad for session notes and helpful context

### Development Workflow
1. **Session Initialization**: At the start of each session, read these files in order:
   - GUIDELINES.md (this file) for project conventions
   - All README.md files in the repository for context and architecture understanding
   - Key header files: `model/game_state.hh`, `view/screen.hh` (this list may expand over time)
2. Use TodoWrite tool to break down complex tasks into manageable steps
3. Research existing codebase before implementing new features
4. Follow existing code conventions and patterns
5. Run lint and typecheck commands after making changes
6. Test implementations when possible
7. **Debug assertion failures**: If an assertion fails while running the game, ALWAYS investigate and fix the root cause rather than ignoring it. Assertion failures indicate logic errors that can lead to undefined behavior or crashes.

### Code Conventions
- Maintain existing code style and patterns
- Check for existing libraries before adding new dependencies
- Follow security best practices
- Avoid adding comments unless explicitly requested
- For template classes: put implementation in `.inl` files and include them in the header to avoid explicit instantiation
- Always use fixed-size integers like uint32_t instead of int
- If a variable should never be negative, use an unsigned integer type
- **Unit suffixes**: All variables with physical units should include unit suffixes in their names (e.g., `duration_ns`, `speed_m_per_s`, `distance_meters`, `angle_radians`)
- **Optional naming**: All std::optional variables should use the `maybe_` prefix (e.g., `maybe_current_path`, `maybe_entity`, `maybe_result`)
- **Entity creation**: Entities should NEVER be constructed directly using `new` or `std::make_unique`. Always use GameState helper methods like `add_entity_and_init<T>()` or `add_child_entity_and_init<T>()`. This ensures entities are properly registered with the game state and can be safely referenced by other systems.
- **SFML abstraction**: SFML types should NEVER be used directly in game code. All SFML functionality must be abstracted through the view classes (e.g., `view::MouseButton`, `view::KeyPressedEvent`) to maintain proper separation of concerns and portability.
- **Variant safety**: std::variant values should NEVER be blindly unwrapped with std::get. Always use std::visit or check std::holds_alternative first to ensure type safety and prevent runtime exceptions.
- **Const correctness**: All variables should be declared const if they are never modified after initialization. This improves code clarity, prevents accidental mutations, and enables compiler optimizations.
- All new classes and functions should have Doxygen-style docstrings that describe:
  - Parameters and return values
  - Assumptions about pre/post conditions
  - Error cases and edge cases with unintuitive behavior

### Unit Testing
- Always use the Catch2 framework for unit tests
- Use `CHECK` macro instead of `REQUIRE` macro unless the test cannot continue if the statement is false
- Using `CHECK` allows us to see more failures in a single test run, improving debugging efficiency
- Only use `REQUIRE` when a failure would make the rest of the test meaningless or cause crashes
- Test files should be placed in appropriate `tests/` directories within each module

### Visual Bug Fixing
- **User feedback requirement**: When fixing visual rendering issues, after each attempt to resolve the problem, ALWAYS ask the user for feedback on how the game currently looks. Visual bugs are difficult to debug without direct observation of the rendered output, so user feedback is essential for iterative improvement.

### Session Management
- Update CLAUDE.md with findings and context for future sessions
- Add new best practices to this file as they're discovered
