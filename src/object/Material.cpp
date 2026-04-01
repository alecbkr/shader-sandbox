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
    programID = "empty";
}


void Material::setName(std::string name) {
    this->name = name;
}


void Material::setProperties(MaterialProperties properties) {
    this->properties = properties;
}


void Material::setProgramID(std::string programID) {
    this->programID = programID;
}


void Material::setMaterialType(MaterialType type) {
    this->type = type;
}


void Material::addTexture(unsigned int textureID) {
    textureIDs.push_back(textureID);
}

std::string Material::getName() {return name; }
std::string Material::getProgramID() { return programID; }
MaterialType Material::getMaterialType() { return type; }
std::vector<unsigned int>& Material::getMaterialTextureIDs() { return textureIDs; }


std::vector<std::string> Material::getAllTexturePaths(TextureCache* texCache) {
    std::vector<std::string> paths;

    for (auto texID : textureIDs) {
        paths.emplace_back(texCache->getTexturePath(texID));
    }

    return paths;
}
