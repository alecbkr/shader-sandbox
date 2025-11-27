#version 330 core
layout (location=0) in vec3 aPos;

uniform float zoom;
uniform ivec3 worldPos;

void main() {
    gl_Position = vec4((aPos.x + worldPos.x)*(1/zoom), (aPos.y+worldPos.y)*(1/zoom), 0.0f, 1.0f);
}