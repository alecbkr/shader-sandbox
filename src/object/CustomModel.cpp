#include "CustomModel.hpp"
#include "texture/TextureCache.hpp"
// #include "texture/CubeMap.hpp"

#include "core/logging/Logger.hpp"

CustomModel::CustomModel(const unsigned int modelID, TextureCache* _textureCachePtr, Logger* _loggerPtr) 
    : Model(modelID, _textureCachePtr, _loggerPtr) {
    primitives.emplace_back(modelID, 0, 0);
    
    all_materials.push_back(std::make_unique<Material>(MaterialType::Opaque, nextMaterialID));
    all_meshes.emplace_back(std::make_unique<MeshA>(nextMeshID));
}

void CustomModel::setMesh(std::vector<float> raw_vertices, std::vector<unsigned int> indices, bool hasPos, bool hasNorm, bool hasUV) {
    all_meshes[0]->unloadFromGPU();

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

    all_meshes[0]->vertices = vertices;
    all_meshes[0]->indices = indices;
    all_meshes[0]->meshflags.hasPositions = hasPos;
    all_meshes[0]->meshflags.hasNormals = hasNorm;
    all_meshes[0]->meshflags.hasUVs = hasUV;
    properties.hasMeshes = true;
}


void CustomModel::addTexture(std::string texture_path, TextureType type) {
    all_materials[0]->assignTexture(textureCachePtr->addTexture(texture_path, type));
}