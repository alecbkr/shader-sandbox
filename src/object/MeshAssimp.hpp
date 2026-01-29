#pragma once

#include "Texture.hpp"
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>

#include "Vertex.hpp"
#include "../engine/ShaderProgram.hpp"


class Model;
class CustomModel;


struct MeshFlags {
    bool hasPositions = false;
    bool hasNormals = false;
    bool hasUVs = false;
    bool hasColors = false;
    bool hasTextures = false;
};


class MeshA {
    
    public:
        MeshA();
        MeshA(std::vector<Vertex> vertices, std::vector<unsigned int> indices, 
              std::vector<std::shared_ptr<Texture>> textures, MeshFlags flags);
    
    private:
        bool isLoadedInGPU = false;
        MeshFlags meshflags;
        
        GLuint vao = 0, vbo = 0, ebo = 0;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<std::shared_ptr<Texture>> textures;
        glm::vec4 baseColor = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
        void bind();
        void unbind();
        void loadToGPU();
        void unloadFromGPU();

        friend class Model;
        friend class CustomModel;
};