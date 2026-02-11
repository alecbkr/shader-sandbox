#pragma once 
#include "../object/Texture.hpp"
#include <vector>

class Logger;

class TextureRegistry {
public:
    TextureRegistry();
    bool initialize(Logger* _loggerPtr);
    void shutdown();
    const Texture* tryReadTexture(int index); // return false if we didn't find it.
    void registerTexture(Texture* texture);
    void eraseTexture(int index);
    const std::vector<const Texture*>& readTextures();

private:
    bool initialized = false;
    Logger* loggerPtr = nullptr;
    std::vector<const Texture*> textures;
};

