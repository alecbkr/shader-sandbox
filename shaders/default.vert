#version 330 core
layout (location=0) in vec3 aPos;

uniform float zoom;
uniform ivec3 worldPos;

void main() {
    float useZoom = zoom;
    if (zoom == 0) useZoom = 1.0;
    gl_Position = vec4((aPos.x + worldPos.x)*(1/zoom), (aPos.y+worldPos.y)*(1/useZoom), 0.0f, 1.0f);
}