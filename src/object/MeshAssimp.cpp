#include "MeshAssimp.hpp"
#include "../engine/Errorlog.hpp"
#include "../texture/TextureType.hpp"
#include "core/logging/Logger.hpp"


MeshA::MeshA(std::vector<Vertex> vertices, std::vector<unsigned int> indices, MeshFlags meshflags, unsigned int id) : ID(id) {
    this->vertices = vertices;
    this->indices = indices;
    this->meshflags = meshflags;
}


MeshA::MeshA(unsigned int id) : ID(id) {

}


MeshA::~MeshA() {
    Logger::addLog(LogLevel::INFO, "MESH", "Deleting mesh...");
    unloadFromGPU();
}


void MeshA::bind() {
    loadToGPU();
    glBindVertexArray(vao);
}


void MeshA::unbind() {
    glBindVertexArray(0);
}


void MeshA::loadToGPU() {
    if (isLoadedInGPU) return;
    
    Logger::addLog(LogLevel::INFO, "MESH", "Loading mesh...");
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), 
                    &indices[0], GL_STATIC_DRAW);
    
    if (meshflags.hasPositions) {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    }

    if (meshflags.hasNormals) {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    }
    
    if (meshflags.hasUVs) {
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    }

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    
    isLoadedInGPU = true;
}


void MeshA::unloadFromGPU() {
    if (!isLoadedInGPU) return;

    Logger::addLog(LogLevel::INFO, "MESH", "Unloading mesh...");
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    vao = 0;
    vbo = 0;
    ebo = 0;
    
    isLoadedInGPU = false;
}