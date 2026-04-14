#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <string>
#include <unordered_set>
#include <filesystem>

#include "texture/Texture.hpp"
#include "texture/TextureType.hpp"
#include "texture/Texture2D.hpp"
#include "texture/CubeMap.hpp"

class Logger;

class TextureCache {
public:
    
private:
    struct TextureInstance {
        std::unique_ptr<Texture> texture;
        unsigned int refCount;
        unsigned int ID;
    };

public:
    bool initialize(Logger *_loggerPtr);
    TextureCache();
    ~TextureCache() = default;

    unsigned int createTexture2D(std::filesystem::path texture2D_path);
    unsigned int createCubeMap(std::vector<std::filesystem::path> texture_paths);
    void deleteTexture(unsigned int ID);
    void bindTexture(unsigned int ID, unsigned int texUnit);
    void bindDefault(unsigned int texUnit);

    std::vector<std::filesystem::path> getTexturePaths(unsigned int textureID);
    unsigned int getTextureTexUnit(unsigned int textureID);
    std::string getName(unsigned int textureID);

private:
    unsigned int nextTextureID = 0;
    std::unique_ptr<Texture> missingTexture2D;
    std::unique_ptr<Texture> missingCubemap;
    std::unordered_map<std::string, std::shared_ptr<TextureInstance>> texturePathMap;
    std::unordered_map<unsigned int, std::shared_ptr<TextureInstance>> textureIDMap;
    std::unordered_set<std::string> allTextureNames;
    // std::unordered_map<unsigned int, std::unique_ptr<TextureInstance>> textureInstanceIDMap;

    TextureInstance* getTextureInstance(unsigned int TextureInstanceID);
    bool assignName(unsigned int textureID, std::string);
    bool validateNextID();

    // component pointers
    Logger* loggerPtr = nullptr;
};
