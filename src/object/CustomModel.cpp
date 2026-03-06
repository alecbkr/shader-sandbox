#include "CustomModel.hpp"
#include "core/logging/LogSink.hpp"
#include "object/MaterialCache.hpp"
#include "texture/TextureCache.hpp"
// #include "texture/CubeMap.hpp"

#include "core/logging/Logger.hpp"

CustomModel::CustomModel(const unsigned int modelID, TextureCache* _textureCachePtr, Logger* _loggerPtr, MaterialCache* _materialCachePtr) 
    : Model(modelID, _textureCachePtr, _loggerPtr, _materialCachePtr) {
    
    auto mat = std::make_unique<Material>(MaterialType::Opaque, _materialCachePtr, ID);
    all_material_ids.push_back(mat->ID);
    primitives.emplace_back(modelID, 0, mat->ID);
    materialCachePtr->createMaterial(std::move(mat));

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
    Material* mat = materialCachePtr->getMaterial(all_material_ids[0]);
    if (mat == nullptr) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "CustomModel::addTexture", "Material[0] not found");
        return;
    }
    mat->assignTexture(textureCachePtr->addTexture(texture_path, type));
}
