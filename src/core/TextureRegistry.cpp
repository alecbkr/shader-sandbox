#include "TextureRegistry.hpp"
#include "logging/Logger.hpp"

TextureRegistry::TextureRegistry() {
}

TextureRegistry& TextureRegistry::instance() {
    static TextureRegistry inst;
    return inst;
}
const Texture* TextureRegistry::tryReadTexture(int index) const {
    if (index < textures.size() && index > -1) {
        Logger::addLog(LogLevel::WARNING, "eraseTexture", "invalid index!"); 
        return textures.at(index);
    }
    else return nullptr;
}

const std::vector<Texture*>& TextureRegistry::readTextures() const {
    return textures;
}

void TextureRegistry::registerTexture(Texture* texture) {
    textures.push_back(texture);
}

void TextureRegistry::eraseTexture(int index) {
    if (index < textures.size() && index > -1) {
        Logger::addLog(LogLevel::WARNING, "eraseTexture", "invalid index!"); 
    }
    textures.erase(textures.begin() + index);
}
