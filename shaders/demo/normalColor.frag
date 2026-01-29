#version 330 core

in vec3 f_normal;

out vec4 FragColor;

void main() {
    FragColor = vec4(f_normal, 1.0f);
}