#pragma once 
#include "../object/Texture.hpp"
#include <vector>

#define TEXTURE_REGISTRY TextureRegistry::instance()

class TextureRegistry {
    public:
    static TextureRegistry& instance();
    const Texture* tryReadTexture(int index) const; // return false if we didn't find it.
    void registerTexture(Texture* texture);
    void eraseTexture(int index);
    const std::vector<Texture*>& readTextures() const;

    private:
    TextureRegistry();
    std::vector<Texture*> textures;
};

