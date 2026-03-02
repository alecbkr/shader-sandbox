#pragma once

#include <vector>
#include <memory>
#include <string>

#include "../texture/Texture.hpp"
#include "../engine/ShaderProgram.hpp"
#include "object/MaterialCache.hpp"

enum class MaterialType {
    Opaque,
    Cutout,
    Translucent,
    Skybox
};

struct MaterialProperties {
    float opacity   = 1.0f;
    float shininess = 0.0f;
    float roughness = 0.0f;
    float metalness = 0.0f;
};

class Material {
    public:
        const unsigned int ID;
        MaterialType type;
        MaterialProperties properties;

        Material(MaterialType type, MaterialCache* materialCachePtr);
        Material(
            MaterialProperties props, 
            std::vector<unsigned int> textures, 
            MaterialType type, 
            MaterialCache* matericalCachePtr
        );
        std::vector<unsigned int> getMaterialTextureIDs();
        void assignTexture(unsigned int textureID);
        void setProgramID(std::string programID);
        std::string getProgramID();
    
    private:
        std::string programID;
        std::vector<unsigned int> textureIDs;

        // SYSTEM POINTERS
};
