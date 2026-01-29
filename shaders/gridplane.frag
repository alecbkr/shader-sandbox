#version 330 core

in vec3 worldPos;
out vec4 FragColor;

vec3 gridColor = vec3(0.2);
float gridScale = 1.0f;

void main() {
    vec2 coord = worldPos.xz * gridScale;

    vec2 grid = abs(fract(coord - 0.5) - 0.5) / fwidth(coord);
    float line = min(grid.x, grid.y);

    float alpha = 1.0 - clamp(line, 0.0, 1.0);
    FragColor = vec4(gridColor, alpha);
}