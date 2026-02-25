#include "Material.hpp"
#include "texture/TextureCache.hpp"
#include "../core/logging/Logger.hpp"


Material::Material(MaterialType type, unsigned int materialID) : ID(materialID) {
    this->type = type;
}


Material::Material(MaterialProperties props, 
    std::vector<unsigned int> textureIDs, 
    MaterialType type, unsigned int materialID
) : ID(materialID)
{
    this->properties = props;
    this->textureIDs = textureIDs;
    this->type = type;
    programID = "empty";
}


void Material::bindTextures() {
    if (textureIDs.empty()) {
        TextureCache::bindDefault();
    }
    else {
        unsigned int texUnit = 0;
        for (unsigned int texID : textureIDs) {
            TextureCache::bindTexture(texID, texUnit++);
        }
    }
}


void Material::assignTexture(unsigned int textureID) {
    textureIDs.push_back(textureID);
}


void Material::setProgramID(std::string programID) {
    this->programID = programID;
}


std::string Material::getProgramID() {
    // Logger::addLog(LogLevel::INFO, "GAYBABY", "little cunt rat", programID);
    return programID;
}