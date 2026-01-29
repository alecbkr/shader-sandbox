#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 worldPos;

void main() {
    vec4 wp = model * vec4(aPos, 1.0f);
    worldPos = wp.xyz;
    gl_Position = projection * model * wp;
}