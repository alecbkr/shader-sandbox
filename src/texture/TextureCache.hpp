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
    public:
        bool initialize(Logger *_loggerPtr);
        TextureCache();
        ~TextureCache() = default;

        unsigned int addTexture(std::string texture_path, TextureType type);
        bool deleteTexture(unsigned int ID);
        bool bindTexture(unsigned int ID, unsigned int texUnit);
        void bindDefault();

    private:
        std::unique_ptr<Texture> defaultTexture;
        unsigned int nextID = 0;
        std::unordered_map<std::string, unsigned int> texturePathMap;
        std::vector<std::shared_ptr<Texture>> textureCache;

        // component pointers
        Logger* loggerPtr = nullptr;
};