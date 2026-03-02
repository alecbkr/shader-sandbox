#pragma once

#include <glad/glad.h>
#include "platform/GL.hpp"
// #include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>

// #include "Vertex.hpp"
// #include "../engine/ShaderProgram.hpp"


class Model;
class CustomModel;


struct MeshFlags {
    bool hasPositions = false;
    bool hasNormals = false;
    bool hasUVs = false;
    bool hasColors = false;
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec4 color = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
};

class MeshA {
    
    public:
        const unsigned int ID;
        MeshA(unsigned int id);
        MeshA(std::vector<Vertex> vertices, std::vector<unsigned int> indices, MeshFlags flags, unsigned int id);
        ~MeshA();
    
    private:
        bool isLoadedInGPU = false;
        MeshFlags meshflags;
        
        GLuint vao = 0, vbo = 0, ebo = 0;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        glm::vec4 baseColor = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
        void bind();
        void unbind();
        void loadToGPU();
        void unloadFromGPU();

        friend class Model;
        friend class CustomModel;
};