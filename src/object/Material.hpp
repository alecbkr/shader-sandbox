#pragma once

#include <vector>
#include <memory>
#include <string>
#include <limits>
#include <unordered_map>

#include "MaterialType.hpp"
#include "MaterialProperties.hpp"

class EventDispatcher;
class TextureCache;

class Material {
public:
    const unsigned int ID;
    
    MaterialProperties properties;

    Material(unsigned int ID, MaterialType type);
    Material(
        unsigned int ID,
        MaterialType type,
        MaterialProperties properties, 
        std::vector<unsigned int> textures
    );

    void setName(std::string name);
    void setProperties(MaterialProperties properties);
    void setMaterialType(MaterialType type);
    void setProgramID(unsigned int programID);
    void setProgramName(std::string programName);
    void addTexture(unsigned int textureID);

    std::string getName();
    unsigned int getProgramID();
    std::string getProgramName();
    MaterialType getMaterialType();
    const bool getValidity() const;
    float getOpacity();
    float getShininess();
    float getRoughness();
    float getMetalness();
    std::vector<unsigned int>& getMaterialTextureIDs();
    // std::unordered_map<unsigned int, std::string> getAllTextureUnitsAndPaths(TextureCache* texCache);
    // std::vector<std::string> getAllTexturePaths(TextureCache* texCache);
    
private:
    std::string name = "material";
    unsigned int programID = std::numeric_limits<unsigned int>::max();
    std::string programName;
    std::vector<unsigned int> textureIDs;
    MaterialType type = MaterialType::Opaque;

    // SYSTEM POINTERS
    EventDispatcher* eventsPtr;
};
