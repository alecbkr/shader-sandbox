#include "TextureCache.hpp"

#include "core/logging/Logger.hpp"


bool TextureCache::initialize(Logger* _loggerPtr) {
    
    loggerPtr = _loggerPtr;

    defaultTexture = std::make_unique<Texture2D>(Texture2D("../assets/textures/default.jpeg", TEX_DIFFUSE, loggerPtr));
    // if (defaultTexture->isValid() == false) {
    //     return false;
    // }
    
    return true;
}


TextureCache::TextureCache() {}


unsigned int TextureCache::addTexture(std::string texture_path, TextureType type) {
    unsigned int texID;

    auto iter = texturePathMap.find(texture_path);
    if (iter != texturePathMap.end()) {
        texID = iter->second;
    }
    else {
        std::shared_ptr<Texture> newTexture;
        switch (type) {
            case TextureType::TEX_CUBEMAP: newTexture = std::make_shared<CubeMap>(texture_path, loggerPtr); break;
            default:                       newTexture = std::make_shared<Texture2D>(texture_path, type, loggerPtr);
        }

        textureCache.emplace_back(newTexture);
        texturePathMap.emplace(texture_path, nextID);
        texID = nextID++;
    }

    return texID;
}


bool TextureCache::deleteTexture(unsigned int ID) {
    if (ID > textureCache.size()) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "TEXTURECACHE | deleteTexture()", " texture ID oob");
        return false;
    }

    std::shared_ptr<Texture> foundTexture = textureCache[ID];
    if (foundTexture == nullptr) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "TEXTURECACHE | deleteTexture()", " texture at ID is nullptr");
        return false;
    }
    
    foundTexture->~Texture();
    textureCache[ID] = nullptr;
    return true;
}


bool TextureCache::bindTexture(unsigned int ID, unsigned int texUnit) {
    if (ID > textureCache.size()) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "TEXTURECACHE | bindTexture()", " texture ID oob");
        return false;
    }

    if (texUnit > 31) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "TEXTURECACHE | bindTexture()", " texUnit must be 0-31");
        return false;
    }

    std::shared_ptr<Texture> foundTexture = textureCache[ID];
    if (foundTexture == nullptr) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "TEXTURECACHE | bindTexture()", " texture at ID is nullptr");
        return false;
    }

    foundTexture->bind(texUnit);
    return true;
}


void TextureCache::bindDefault() {
    defaultTexture->bind(0);
}