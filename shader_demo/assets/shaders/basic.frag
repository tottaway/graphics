#version 330 core

// Inputs from vertex shader
in vec2 world_position;

// Test uniforms
uniform float time;              // For animation testing
uniform vec2 test_light_pos;     // Single test light position
uniform vec3 test_light_color;   // Test light color

// Output color
out vec4 fragment_color;

void main() {
    // Start with dark ambient
    vec3 ambient = vec3(0.05, 0.05, 0.1);
    vec3 final_color = ambient;

    // Calculate distance to test light
    float distance = length(world_position - test_light_pos);
    float light_radius = 1.0;

    // Simple circular light with smooth falloff
    if (distance < light_radius) {
        float attenuation = 1.0 - smoothstep(0.0, light_radius, distance);
        final_color += test_light_color * attenuation;
    }

    // Add a simple animation effect (pulsing based on time)
    final_color *= (0.8 + 0.2 * sin(time));

    fragment_color = vec4(final_color, 1.0);
}