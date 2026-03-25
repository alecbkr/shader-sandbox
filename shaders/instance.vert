#version 330 core
layout (location=0) in vec3 aPos;
layout (location=2) in vec2 aTexCoord;
layout (location=3) in vec4 aColor;
layout (location=4) in vec3 aInstance;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 TexCoord;
out vec4 fragColor;


void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);
    worldPos.xyz += aInstance;

    gl_Position = projection * view * worldPos;
    
    TexCoord = aTexCoord;
    fragColor = aColor;

}
