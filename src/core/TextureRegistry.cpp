#include "TextureRegistry.hpp"
#include "../engine/Errorlog.hpp"

std::vector<const Texture*> TextureRegistry::textures;

const Texture* TextureRegistry::tryReadTexture(int index) {
    if ((long long unsigned int)index < textures.size() && index > -1) {
        ERRLOG.logEntry(EL_WARNING, "eraseTexture", "invalid index!");
        return textures.at(index);
    }
    else return nullptr;
}

const std::vector<const Texture*>& TextureRegistry::readTextures() {
    return textures;
}

void TextureRegistry::registerTexture(Texture* texture) {
    textures.push_back(texture);
}

void TextureRegistry::eraseTexture(int index) {
    if ((long long unsigned int)index < textures.size() && index > -1) {
        ERRLOG.logEntry(EL_WARNING, "eraseTexture", "invalid index!");
    }
    textures.erase(textures.begin() + index);
}
