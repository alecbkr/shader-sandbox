#include "CustomModel.hpp"

#include "core/logging/Logger.hpp"

CustomModel::CustomModel(const unsigned int ID, ShaderRegistry* shaderRegPtr, Logger* _loggerPtr)
    : Model(ID, shaderRegPtr, _loggerPtr), loggerPtr(_loggerPtr) {
    all_meshes.push_back(MeshA());
    meshptr = &all_meshes.back();
}

void CustomModel::setMesh(std::vector<float> raw_vertices, std::vector<unsigned int> indices, bool hasPos, bool hasNorm, bool hasUV) {
    meshptr->unloadFromGPU();

    unsigned int rowstride = 0;
    rowstride += 3*hasPos;
    rowstride += 3*hasNorm;
    rowstride += 2*hasUV;

    if (raw_vertices.size() % rowstride != 0) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MODEL_ADDMESH", "passed vertex data is wrong");
        return;
    }

    // VERTICES
    std::vector<Vertex> vertices;
    for (unsigned int row = 0; row < raw_vertices.size() / rowstride; row++) {
        Vertex vertex;
        unsigned int col = 0;

        if (hasPos) {
            glm::vec3 vector3;
            vector3.x = raw_vertices.at((col++) + (row*rowstride));
            vector3.y = raw_vertices.at((col++) + (row*rowstride));
            vector3.z = raw_vertices.at((col++) + (row*rowstride));
            vertex.position = vector3;
        }

        if (hasNorm) {
            glm::vec3 vector3;
            vector3.x = raw_vertices.at((col++) + (row*rowstride));
            vector3.y = raw_vertices.at((col++) + (row*rowstride));
            vector3.z = raw_vertices.at((col++) + (row*rowstride));
            vertex.normal = vector3;
        }

        if (hasUV) {
            glm::vec2 vector2;
            vector2.x = raw_vertices.at((col++) + (row*rowstride));
            vector2.y = raw_vertices.at((col++) + (row*rowstride));
            vertex.uv = vector2;
        }

        vertices.push_back(vertex);
    }

    meshptr->vertices = vertices;
    meshptr->indices = indices;

    meshptr->meshflags.hasPositions = hasPos;
    meshptr->meshflags.hasNormals = hasNorm;
    meshptr->meshflags.hasUVs = hasUV;

    properties.hasMeshes = true;
}


void CustomModel::addTexture(std::string filepath) {

}