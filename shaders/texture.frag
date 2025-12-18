#version 330 core

in vec2 TexCoord;

uniform sampler2D baseTex;
uniform sampler2D outlineTex;

out vec4 FragColor;
void main() {


    float thresh = 0.05;
    vec4 base = texture(baseTex, TexCoord);
    vec4 outline = texture(outlineTex, TexCoord);
    
    bool isBlack = outline.r < thresh && outline.b < thresh && outline.g < thresh;

    FragColor = texture(baseTex, TexCoord);
    FragColor = isBlack ? base : outline;
}