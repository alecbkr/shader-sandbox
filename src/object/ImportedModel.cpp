#include "ImportedModel.hpp"

#include "ModelImporter.hpp"
#include "../engine/Errorlog.hpp"


ImportedModel::ImportedModel(const unsigned int ID, std::string pathname) : Model(ID) {
    
    if (importModel(pathname, *this) == false) {
        ERRLOG.logEntry(EL_ERROR, "MODEL", "Model import failed, returned false");
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
