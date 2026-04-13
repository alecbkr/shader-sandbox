#include "TextureCache.hpp"

#include "core/logging/Logger.hpp"
#include "TextureStatus.hpp"


bool TextureCache::initialize(Logger* _loggerPtr) {
    
    loggerPtr = _loggerPtr;

    defaultTexture = std::make_unique<Texture2D>(Texture2D("../assets/textures/default.jpeg"));
    // defaultTexture = std::make_unique<Texture2D>(Texture2D("../assets/textures/water.png", TEX_DIFFUSE, loggerPtr));
    // if (defaultTexture->isValid() == false) {
    //     return false;
    // }
    
    return true;
}


TextureCache::TextureCache() {}


unsigned int TextureCache::createTexture2D(std::string texture2D_path) {
    unsigned int newTextureID;

    auto iter = texturePathMap.find(texture2D_path);
    if (iter != texturePathMap.end()) {
        iter->second.get()->refCount++;
        newTextureID = iter->second.get()->ID;
    }
    else {
        if (validateNextID() == false) {
            loggerPtr->addLog(LogLevel::LOG_ERROR, "createTexture2D", "could not validate nextMaterialInstanceID");
        }

        newTextureID = nextTextureID;
        auto textureInstancePtr = std::make_shared<TextureInstance>(std::make_unique<Texture2D>(texture2D_path), 1, newTextureID);
        textureIDMap.emplace(newTextureID, textureInstancePtr);
        texturePathMap.emplace(texture2D_path, textureInstancePtr);
        nextTextureID++;
    }
    return newTextureID;
}


unsigned int TextureCache::createCubeMap(std::vector<std::string> texture_paths) {
    unsigned int newTextureID;

    // auto iter = texturePathMap.find(cubemap_dir_path);
    // if (iter != texturePathMap.end()) {
    //     iter->second.get()->refCount++;
    //     newTextureID = iter->second.get()->ID;
    // }
    
    if (validateNextID() == false) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "createTexture2D", "could not validate nextMaterialInstanceID");
        return std::numeric_limits<unsigned int>::max();
    }

    newTextureID = nextTextureID;
    auto textureInstancePtr = std::make_shared<TextureInstance>(std::make_unique<CubeMap>(texture_paths), 1, newTextureID);
    textureIDMap.emplace(newTextureID, textureInstancePtr);
    // texturePathMap.emplace(cubemap_dir_path, textureInstancePtr);
    nextTextureID++;
    
    return newTextureID;
}


void TextureCache::deleteTexture(unsigned int textureID) {
    TextureInstance* foundTextureInstance = getTextureInstance(textureID);
    if (foundTextureInstance == nullptr) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "TEXTURECACHE | deleteTexture()", " textureID out of bounds: ", std::to_string(textureID));
        return;
    }

    if ((--foundTextureInstance->refCount) == 0) {
        if (foundTextureInstance->texture->paths[1].empty()) {
        texturePathMap.erase(foundTextureInstance->texture->paths[0]);
        }
        textureIDMap.erase(textureID);
    }
}


void TextureCache::bindTexture(unsigned int textureID, unsigned int texUnit) {
    TextureInstance* foundTextureInstance = getTextureInstance(textureID);
    if (foundTextureInstance == nullptr) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "TEXTURECACHE | bindTexture()", " textureInstanceID out of bounds: ", std::to_string(textureID));
        return;
    }

    if (texUnit > 31) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "TEXTURECACHE | bindTexture()", " texUnit must be 0-31");
        return;
    }

    Texture* foundTexture = foundTextureInstance->texture.get();
    // Silently fail because texture is in error state
    if (foundTexture->getStatus() == TextureStatus::FileNotFound || foundTexture->getStatus() == TextureStatus::InvalidFormat) {
        bindDefault(texUnit);
        return;
    }

    if (foundTexture->bind(texUnit) == false) {
        switch (foundTexture->getStatus()) {
            case TextureStatus::FileNotFound: 
                loggerPtr->addLog(LogLevel::LOG_ERROR, "TEXTURECACHE | bindTexture()", " texture path(s) could not be found");
                break;
            case TextureStatus::InvalidFormat:
                loggerPtr->addLog(LogLevel::LOG_ERROR, "TEXTURECACHE | bindTexture()", " texUnit must be 0-31");
                break;
            default:
                break;
        }
        bindDefault(texUnit);
    }
}


void TextureCache::bindDefault(unsigned int texUnit) {
    defaultTexture->bind(texUnit);
}


TextureCache::TextureInstance* TextureCache::getTextureInstance(unsigned int textureID) {
    auto iter = textureIDMap.find(textureID);
    if (iter == textureIDMap.end()) {
        return nullptr;
    }
    return iter->second.get();
}


bool TextureCache::validateNextID() {
    unsigned int numOfChecks = 0;
    while (textureIDMap.contains(nextTextureID) == true) {
        if (numOfChecks > 1000) return false;
        
        nextTextureID++;
        numOfChecks++;
    }
    return true;
}


std::vector<std::string> TextureCache::getTexturePaths(unsigned int textureID) {
    if (textureIDMap.contains(textureID) == false) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "TEXTURECACHE | getTexturePath", "texture not found with ID " + std::to_string(textureID));
        std::vector<std::string> emptypaths;
        return emptypaths;
    }

    return textureIDMap.at(textureID)->texture->paths;
}

unsigned int TextureCache::getTextureTexUnit(unsigned int textureID) {
    if (!textureIDMap.contains(textureID)) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "TEXTURECACHE | getTextureTexUnit", "texture not found with ID " + std::to_string(textureID));
        return 0;
    }

    return textureIDMap.at(textureID)->texture->getTexUnit();
}