#version 330 core

// Input from vertex shader
in vec2 world_position;

// Output color
out vec4 FragColor;

// Light configuration
const int MAX_LIGHTS = 32;
uniform int light_count;                    // Actual number of lights
uniform vec2 light_positions[MAX_LIGHTS];   // Light positions in world coordinates
uniform vec3 light_colors[MAX_LIGHTS];      // Light RGB colors (0-255 range)
uniform float light_radii[MAX_LIGHTS];     // Light radii in world units

void main() {
    // Start with no light (black multiplier)
    vec3 total_light_color = vec3(0.0, 0.0, 0.0);

    // Accumulate light contributions with their colors
    for (int i = 0; i < light_count && i < MAX_LIGHTS; i++) {
        // Calculate distance from this pixel to the light
        float distance = length(world_position - light_positions[i]);

        // Calculate light intensity with smooth falloff
        if (distance < light_radii[i]) {
            // Quadratic falloff for realistic lighting
            float normalized_distance = distance / light_radii[i];
            float intensity = 1.0 - (normalized_distance * normalized_distance);

            // Convert light color from 0-255 range to 0-1 range and apply intensity
            vec3 light_contribution = (light_colors[i] / 255.0) * intensity;

            // Additive blending - lights combine naturally
            total_light_color += light_contribution;
        }
    }

    // Clamp to prevent overbrightening
    total_light_color = min(total_light_color, vec3(1.0, 1.0, 1.0));

    // For multiplicative blending:
    // - Black areas (no light) = vec3(0,0,0) which will make underlying content black
    // - Lit areas = light color which will tint the underlying content
    // - Use alpha to control the strength of the lighting effect
    FragColor = vec4(total_light_color, 1.0);
}