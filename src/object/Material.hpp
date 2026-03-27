#pragma once

#include <vector>
#include <memory>
#include <string>

#include "MaterialType.hpp"
#include "MaterialProperties.hpp"

class EventDispatcher;
class TextureCache;

class Material {
public:
    const unsigned int ID;
    std::string name = "material";
    
    MaterialProperties properties;

    Material(unsigned int ID, MaterialType type);
    Material(
        unsigned int ID,
        MaterialType type,
        MaterialProperties properties, 
        std::vector<unsigned int> textures
    );


    void setProperties(MaterialProperties properties);
    void setMaterialType(MaterialType type);
    void setProgramID(std::string programID);
    void addTexture(unsigned int textureID);

    std::string getProgramID();
    MaterialType getMaterialType();
    float getOpacity();
    float getShininess();
    float getRoughness();
    float getMetalness();
    std::vector<unsigned int>& getMaterialTextureIDs();
    std::vector<std::string> getAllTexturePaths(TextureCache* texCache);
    
private:
    std::string programID;
    std::vector<unsigned int> textureIDs;
    MaterialType type = MaterialType::Opaque;

    // SYSTEM POINTERS
    EventDispatcher* eventsPtr;
};
