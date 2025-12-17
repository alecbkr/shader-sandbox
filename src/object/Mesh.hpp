#ifndef MESH_HPP
#define MESH_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
    glm::vec3 position;
};


class Mesh {
    public:
        bool hasNorms = false;
        bool hasUVs = false;
        bool hasIndices = false;
        bool loadedInGPU = false;
        GLuint vao;
        GLuint vbo;
        GLuint ebo;
        std::vector<float> vertices; // TODO requested of vertex type
        std::vector<int> indices;
        Mesh(std::vector<float> verticesIn);
        Mesh(std::vector<float> verticesIn, std::vector<int> indicesIn, bool hasNorms, bool hasUVs);
        void sendToGPU();
        void draw();
};

#endif