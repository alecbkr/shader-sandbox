#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

#include "Texture.hpp"
#include "TextureType.hpp"
#include "Texture2D.hpp"
#include "CubeMap.hpp"


class TextureCache {
    public:
        static bool initialize();
        TextureCache() = delete;
        ~TextureCache() = default;

        static unsigned int addTexture(std::string texture_path, TextureType type);
        static bool deleteTexture(unsigned int ID);
        static bool bindTexture(unsigned int ID, unsigned int texUnit);
        static void bindDefault();

    private:
        static Texture2D defaultTexture;
        static unsigned int nextID;
        static std::unordered_map<std::string, unsigned int> texturePathMap;
        static std::vector<std::shared_ptr<Texture>> textureCache;
};