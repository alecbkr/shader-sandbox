#pragma once 
#include "../object/Texture.hpp"
#include <vector>

class TextureRegistry {
public:
    static const Texture* tryReadTexture(int index); // return false if we didn't find it.
    static void registerTexture(Texture* texture);
    static void eraseTexture(int index);
    static const std::vector<const Texture*>& readTextures();

private:
    static std::vector<const Texture*> textures;
};

