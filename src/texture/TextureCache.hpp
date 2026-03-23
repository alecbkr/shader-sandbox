#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

#include "Texture.hpp"
#include "TextureType.hpp"
#include "Texture2D.hpp"
#include "CubeMap.hpp"

class Logger;

class TextureCache {
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

    unsigned int createTexture2D(std::string texture2D_path);
    unsigned int createCubeMap(std::string cubemap_dir_path);
    void deleteTexture(unsigned int ID);
    void bindTexture(unsigned int ID, unsigned int texUnit);
    void bindDefault(unsigned int texUnit);

    std::string getTexturePath(unsigned int textureID);

private:
    unsigned int nextTextureID = 0;
    std::unique_ptr<Texture> defaultTexture;
    std::unordered_map<std::string, std::shared_ptr<TextureInstance>> texturePathMap;
    std::unordered_map<unsigned int, std::shared_ptr<TextureInstance>> textureIDMap;
    // std::unordered_map<unsigned int, std::unique_ptr<TextureInstance>> textureInstanceIDMap;

    TextureInstance* getTextureInstance(unsigned int TextureInstanceID);
    bool validateNextID();

    // component pointers
    Logger* loggerPtr = nullptr;
};
