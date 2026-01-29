#version 330 core

uniform sampler2D base;

in vec2 TexCoord;
in vec4 fragColor;

out vec4 FragColor;

void main() {

    //FragColor = vec4(TexCoord, 0.5, 1.0);
    FragColor = texture(base, TexCoord);
}
