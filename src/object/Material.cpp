#include "Material.hpp"

#include "texture/TextureCache.hpp"

Material::Material(unsigned int ID, MaterialType type) 
    : ID(ID)
{
    
}


Material::Material(unsigned int ID, MaterialType type, MaterialProperties props, 
    std::vector<unsigned int> textureIDs) : ID(ID)
{   
    this->properties = props;
    this->textureIDs = textureIDs;
    this->type = type;
}


void Material::setName(std::string name) {
    this->name = name;
}


void Material::setProperties(MaterialProperties properties) {
    this->properties = properties;
}


void Material::setProgramID(unsigned int programID) {
    this->programID = programID;
}

void Material::setProgramName(std::string programName) {
    this->programName = programName;
}


void Material::setMaterialType(MaterialType type) {
    this->type = type;
}


void Material::addTexture(unsigned int textureID) {
    for (unsigned int existingID : textureIDs) {
        if (textureID == existingID) return;
    }
    
    textureIDs.push_back(textureID);
}

std::string Material::getName() {return name; }
unsigned int Material::getProgramID() { return programID; }
std::string Material::getProgramName() { return programName; }
MaterialType Material::getMaterialType() { return type; }
std::vector<unsigned int>& Material::getMaterialTextureIDs() { return textureIDs; }

const bool Material::getValidity() const  { 
    bool result;
    programID != std::numeric_limits<unsigned int>::max() ? result = true : result = false;
    return result;
};

// std::unordered_map<unsigned int, std::string> Material::getAllTextureUnitsAndPaths(TextureCache* texCache) {
//     std::unordered_map<unsigned int, std::string> data;

//     for (auto texID : textureIDs) {
//         data.emplace(texCache->getTextureTexUnit(texID), texCache->getTexturePath(texID));
//     }

//     return data;
// }

// std::vector<std::string> Material::getAllTexturePaths(TextureCache* texCache) {
//     std::vector<std::string> paths;

//     for (auto texID : textureIDs) {
//         paths.emplace_back(texCache->getTexturePath(texID));
//     }

//     return paths;
// }
