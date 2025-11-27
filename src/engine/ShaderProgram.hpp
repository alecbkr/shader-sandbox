#ifndef SHADERPROGRAM_HPP
#define SHADERPROGRAM_HPP

#include <glad/glad.h>
#include <glfw/glfw3.h>

class ShaderProgram {
    public:
        GLuint ID;
        ShaderProgram(const char *vertshader_path, const char *fragshader_path);
        void kill();
        void use();
        void setFloat(const char *uniformName, float val);
        void setVec3f(const char *uniformName, float xVal, float yVal, float zVal);
        void setVec3i(const char *uniformName, int xVal, int yVal, int zVal);
};

#endif