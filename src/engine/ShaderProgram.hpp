#ifndef SHADERPROGRAM_HPP
#define SHADERPROGRAM_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


class ShaderProgram {
    public:
        GLuint ID;
        ShaderProgram(const char *vertshader_path, const char *fragshader_path);
        void use();
        void kill();
        void setUniform_int(const char *uniformName, int val);
        void setUniform_float(const char *uniformName, float val);
        void setUniform_vec3int(const char *uniformName, int xVal, int yVal, int zVal);
        void setUniform_vec3int(const char *uniformName, glm::ivec3 vals);
        void setUniform_vec3float(const char *uniformName, float xVal, float yVal, float zVal);
        void setUniform_vec3float(const char *uniformName, glm::fvec3 vals);
        void setUniform_mat4float(const char *uniformName, glm::fmat4 vals) const;
};

#endif