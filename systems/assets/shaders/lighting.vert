#version 330 core

// Vertex attributes for fullscreen quad
layout (location = 0) in vec2 position;  // Screen-space position (-1 to 1)

// Camera/viewport uniforms
uniform vec2 viewport_center;    // Camera position in world coordinates
uniform vec2 viewport_size;      // Size of viewport in world units

// Outputs to fragment shader
out vec2 world_position;         // World coordinates for this pixel

void main() {
    // Convert from screen-space quad to world coordinates
    // This allows the fragment shader to work in world space for lighting calculations
    world_position = viewport_center + (position * viewport_size * 0.5);

    // Standard fullscreen quad vertex transformation
    gl_Position = vec4(position, 0.0, 1.0);
}