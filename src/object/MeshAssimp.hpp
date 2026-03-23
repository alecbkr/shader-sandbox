#pragma once

#include <glad/glad.h>
#include "platform/GL.hpp"
#include "InstanceData.hpp"
// #include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>
#include "Vertex.hpp"
#include "MeshProperties.hpp"

class MeshA {
    
    public:
        const unsigned int ID;
        MeshA(unsigned int meshID);
        MeshA(unsigned int meshID, std::vector<Vertex> vertices, std::vector<unsigned int> indices, bool hasPos, bool hasNorms, bool hasUVs);
        ~MeshA();

        void bind();
        void unbind();
    
    private:
        bool isLoadedInGPU = false;
        MeshProperties meshflags;
        
        GLuint vao = 0, vbo = 0, ebo = 0;
        unsigned int meshInstanceCount = 1;
        GLuint instanceVBO = 0;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        glm::vec4 baseColor = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
        
        void loadToGPU();
        void unloadFromGPU();
        void setInstanceVBO(unsigned int modelInstanceCount, std::vector<InstanceData> instanceData);

        friend class Model;
        friend class CustomModel;
};