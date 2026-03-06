#include "ImportedModel.hpp"

#include "AssimpImporter.hpp"
#include "core/logging/Logger.hpp"
#include "object/MaterialCache.hpp"


ImportedModel::ImportedModel(const unsigned int ID, std::string pathname, TextureCache* _textureCachePtr, Logger* _loggerPtr, MaterialCache* _materialCachePtr)
    : Model(ID, _textureCachePtr, _loggerPtr, _materialCachePtr) {
    
    if (importModel(pathname, *this, textureCachePtr, loggerPtr, materialCachePtr) == false) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "MODEL", "Model import failed, returned false");
        return;
    }
    all_meshes.empty() == true ? properties.hasMeshes = false : properties.hasMeshes = true;
    // all_textures.empty() == true ? properties.hasTextures = false : properties.hasTextures = true;
}


void ImportedModel::addMesh(std::vector<Vertex> vertices, std::vector<unsigned int>indices, MeshFlags meshflags) {
    all_meshes.emplace_back(std::make_unique<MeshA>(vertices, indices, meshflags, nextMeshID++));
}


void ImportedModel::addMaterial(MaterialProperties properties, std::vector<unsigned int> textureIDs, MaterialType type) {

    auto mat = std::make_unique<Material>(properties, textureIDs, type, materialCachePtr, ID);
    all_material_ids.emplace_back(mat->ID);
    materialCachePtr->createMaterial(std::move(mat));
}


void ImportedModel::addTexture(std::string texture_path, TextureType type) {
    // if (all_textures.contains(texture_path)) return; // Already contains texture

    // std::shared_ptr<Texture> newTexture = loadTexture(texture_path, type);
    // all_textures.emplace(texture_path, newTexture);
}


unsigned int ImportedModel::getID() const {
    return ID;
}


std::vector<std::unique_ptr<MeshA>>& ImportedModel::getMeshVec() {
    return all_meshes;
}


std::vector<unsigned int>& ImportedModel::getMatVec() {
    return all_material_ids;
}


// std::unordered_map<std::string, std::shared_ptr<Texture>>& ImportedModel::getTextures() {
//     return all_textures;
// }
