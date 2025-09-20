# Project Guidelines

## Best Practices for Working on This Project

### File Organization
- **GUIDELINES.md**: Contains project best practices and conventions (this file)
- **TODO.md**: High-level items and features to implement
- **CLAUDE.md**: External scratch pad for session notes and helpful context

### Development Workflow
1. Read README.md files within the project at the start of each session for context
2. Check TODO.md for current high-level objectives
3. Use TodoWrite tool to break down complex tasks into manageable steps
4. Research existing codebase before implementing new features
5. Follow existing code conventions and patterns
6. Run lint and typecheck commands after making changes
7. Test implementations when possible
8. **Debug assertion failures**: If an assertion fails while running the game, ALWAYS investigate and fix the root cause rather than ignoring it. Assertion failures indicate logic errors that can lead to undefined behavior or crashes.

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
- All new classes and functions should have Doxygen-style docstrings that describe:
  - Parameters and return values
  - Assumptions about pre/post conditions
  - Error cases and edge cases with unintuitive behavior

### Session Management
- Update CLAUDE.md with findings and context for future sessions
- Add new best practices to this file as they're discovered
- Keep TODO.md updated with completed and new items