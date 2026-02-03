#version 330 core

uniform vec3 cameraPos;

in vec3 worldPos;
out vec4 FragColor;

// VARIABLES
vec3 gridColor = vec3(0.2);
float gridScale = 1.0f;
float fadeStart = 5.0;
float fadeEnd = 20.0;

void main() {

    float fragDistance = distance(cameraPos, worldPos);
    float fade = 1.0 - smoothstep(fadeStart, fadeEnd, fragDistance);
    if (fade < 0.01) {
        discard;
    }
    

    vec2 coord = worldPos.xz * gridScale;

    vec2 grid = abs(fract(coord - 0.5) - 0.5) / fwidth(coord);
    float line = min(grid.x, grid.y);

    float alpha = (1.0 - clamp(line, 0.0, 1.0)) * fade;
    if (alpha < 0.01) {
        discard;
    }
    FragColor = vec4(gridColor, alpha);
}