#version 330 core

uniform vec3 inColor;

out vec4 FragColor;
void main() {
    FragColor = vec4(inColor, 1.0f);
    FragColor = vec4(1.0f, 0.5f, 1.0f, 1.0f);
}
