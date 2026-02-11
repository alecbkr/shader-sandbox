#include "ImportedModel.hpp"

#include "object/ModelImporter.hpp"
#include "core/logging/Logger.hpp"


ImportedModel::ImportedModel(const unsigned int ID, std::string pathname, ModelImporter* _modelImporterPtr, ShaderRegistry* _shaderRegPtr, Logger* _loggerPtr)
    : Model(ID, _shaderRegPtr, _loggerPtr) {
    
    if (_modelImporterPtr->importModel(pathname, *this) == false) {
        _loggerPtr->addLog(LogLevel::LOG_ERROR, "MODEL", "Model import failed, returned false");
        return;
    }
    all_meshes.empty() == true ? properties.hasMeshes = false : properties.hasMeshes = true;
    all_textures.empty() == true ? properties.hasTextures = false : properties.hasTextures = true;
}


std::vector<MeshA>& ImportedModel::getMeshes() {
    return all_meshes;
}


std::unordered_map<std::string, std::shared_ptr<Texture>>& ImportedModel::getTextures() {
    return all_textures;
}
