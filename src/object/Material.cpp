#include "Material.hpp"
#include "MaterialCache.hpp"
#include "texture/TextureCache.hpp"
#include "../core/logging/Logger.hpp"


Material::Material(MaterialType type, MaterialCache* materialCachePtr, unsigned int _modelID) : ID(materialCachePtr->getNextMaterialID()), modelID(_modelID) {
    this->type = type;
}


Material::Material(MaterialProperties props, 
    std::vector<unsigned int> textureIDs, 
    MaterialType type, MaterialCache* materialCachePtr, unsigned int _modelID
) : ID(materialCachePtr->getNextMaterialID()), modelID(_modelID)
{
    this->properties = props;
    this->textureIDs = textureIDs;
    this->type = type;
    programID = "empty";
}


std::vector<unsigned int> Material::getMaterialTextureIDs() {
    // if (textureIDs.empty()) {
    //     TextureCache::bindDefault();
    // }
    // else {
    //     unsigned int texUnit = 0;
    //     for (unsigned int texID : textureIDs) {
    //         TextureCache::bindTexture(texID, texUnit++);
    //     }
    // }

    return textureIDs;
}


void Material::assignTexture(unsigned int textureID) {
    textureIDs.push_back(textureID);
}


void Material::setProgramID(std::string programID) {
    this->programID = programID;
}


std::string Material::getProgramID() {
    return programID;
}
