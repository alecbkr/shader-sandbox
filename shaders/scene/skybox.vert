#version 330 core
layout (location=0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;

out vec3 TexCoord;


void main() {
    
    mat4 rotationalView = mat4(mat3(view));

    TexCoord = aPos;
    gl_Position = projection*rotationalView * vec4(aPos, 1.0f);
    gl_Position.z = gl_Position.w;
}