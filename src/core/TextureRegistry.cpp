#include "TextureRegistry.hpp"
#include "core/logging/Logger.hpp"


TextureRegistry::TextureRegistry() {
    initialized = false;
    loggerPtr = nullptr;
    textures.clear();
}

bool TextureRegistry::initialize(Logger* _loggerPtr) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Texture Registry Initialization", "Texture Registry is already initialized.");
        return false;
    }
    loggerPtr = _loggerPtr;
    textures.clear();
    initialized = true;
    return true;
}

void TextureRegistry::shutdown() {
    if (!initialized) return;
    loggerPtr = nullptr;
    textures.clear();
    initialized = false;
}

const Texture* TextureRegistry::tryReadTexture(int index) {
    if ((long long unsigned int)index < textures.size() && index > -1) {
        loggerPtr->addLog(LogLevel::WARNING, "eraseTexture", "invalid index!");
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
        loggerPtr->addLog(LogLevel::WARNING, "eraseTexture", "invalid index!");
    }
    textures.erase(textures.begin() + index);
}
