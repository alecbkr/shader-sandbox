#include "Material.hpp"


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


std::string Material::getProgramID() { return programID; }
MaterialType Material::getMaterialType() { return type; }
std::vector<unsigned int>& Material::getMaterialTextureIDs() { return textureIDs; }

