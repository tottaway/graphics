#version 330 core

// Vertex attributes
layout (location = 0) in vec2 position;  // Screen-space position (-1 to 1)

// Uniforms
uniform vec2 viewport_center;    // Camera position in world
uniform vec2 viewport_size;      // Size of viewport in world units

// Outputs to fragment shader
out vec2 world_position;         // World coordinates for this pixel

void main() {
    // Convert from screen quad to world coordinates
    world_position = viewport_center + (position * viewport_size * 0.5);

    // Pass through position for rendering
    gl_Position = vec4(position, 0.0, 1.0);
}