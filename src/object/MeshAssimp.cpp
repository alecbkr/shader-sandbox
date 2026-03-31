#include "MeshAssimp.hpp"
#include "../engine/Errorlog.hpp"
#include "../texture/TextureType.hpp"
#include "core/logging/Logger.hpp"

#include <iostream>

MeshA::MeshA( unsigned int meshIdx, std::vector<Vertex> vertices, std::vector<unsigned int> indices, 
    bool hasPos, bool hasNorms, bool hasUVs) : ID(meshIdx) {

    this->vertices = vertices;
    this->indices = indices;
    this->meshflags.hasPositions = hasPos;
    this->meshflags.hasNormals = hasNorms;
    this->meshflags.hasUVs = hasUVs;
}


MeshA::MeshA(unsigned int id) : ID(id) {

}


MeshA::~MeshA() {
    // Logger::addLog(LogLevel::INFO, "MESH", "Deleting mesh...");
    unloadFromGPU();
}


void MeshA::bind(std::vector<InstanceData>& instanceData) {
    loadToGPU(instanceData);
    glBindVertexArray(vao);
}


void MeshA::unbind() {
    glBindVertexArray(0);
}


void MeshA::loadToGPU(std::vector<InstanceData>& instanceData) {
    if (isLoadedInGPU) return;
    
    // Logger::addLog(LogLevel::INFO, "MESH", "Loading mesh...");
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

    resizeInstanceVBO(instanceData);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)0);
    glVertexAttribDivisor(4, 1);
    
    isLoadedInGPU = true;
}


void MeshA::unloadFromGPU() {
    if (!isLoadedInGPU) return;

    // Logger::addLog(LogLevel::INFO, "MESH", "Unloading mesh...");
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    vao = 0;
    vbo = 0;
    ebo = 0;
    
    isLoadedInGPU = false;
}


void MeshA::resizeInstanceVBO(std::vector<InstanceData>& instanceData) {
    if (instanceVBO == 0) {
        glGenBuffers(1, &instanceVBO);
    }
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, instanceData.size() * sizeof(InstanceData), nullptr, GL_DYNAMIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, instanceData.size() * sizeof(InstanceData), instanceData.data());
}


void MeshA::updateInstanceData(std::vector<InstanceData>& instanceData) {
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, instanceData.size()* sizeof(InstanceData), instanceData.data());
}