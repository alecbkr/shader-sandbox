#include "Mesh.hpp"
#include "../engine/Errorlog.hpp"

Mesh::Mesh(std::vector<float> verticesIn) {
    hasIndices = false;
    vertices = verticesIn;
}

Mesh::Mesh(std::vector<float> verticesIn, std::vector<int> indicesIn, bool hasNorms, bool hasUVs) {
    hasIndices = true;
    this->hasNorms = hasNorms;
    this->hasUVs = hasUVs;
    vertices = verticesIn;
    indices = indicesIn;
}

void Mesh::sendToGPU() {
    int rowStride = 3;
    if (hasNorms) rowStride += 3;
    if (hasUVs)   rowStride += 2;
    

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);

    if (hasIndices) {
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(int), indices.data(), GL_STATIC_DRAW);
    }

    // VERTICE POSITION
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, rowStride*sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // NORMALS
    if (hasNorms) {
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, rowStride*sizeof(GLfloat), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    // TEXTURE UVs
    if (hasUVs) {
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, rowStride*sizeof(GLfloat), (void*)((rowStride - 2)*sizeof(float)));
        glEnableVertexAttribArray(2);
    }

    glBindVertexArray(0);
    loadedInGPU = true;
}

void Mesh::draw() {
    if (!loadedInGPU) {
        sendToGPU();
    }

    glBindVertexArray(vao);
    if (hasIndices) {
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    }
    else {
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    }
    glBindVertexArray(0);
}