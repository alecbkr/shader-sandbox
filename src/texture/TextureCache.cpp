#include "TextureCache.hpp"

#include <algorithm>
#include "core/logging/Logger.hpp"
#include "TextureStatus.hpp"


bool TextureCache::initialize(Logger* _loggerPtr) {
    
    loggerPtr = _loggerPtr;
    std::vector<std::string> missingTexturePaths(6, "../assets/textures/missingTexture.png");

    missingTexture2D = std::make_unique<Texture2D>(Texture2D(missingTexturePaths[0]));
    missingCubemap = std::make_unique<CubeMap>(CubeMap(missingTexturePaths));
    
    return true;
}


TextureCache::TextureCache() {}


unsigned int TextureCache::createTexture2D(std::filesystem::path texture2D_path) {
    unsigned int newTextureID;

    auto iter = texturePathMap.find(texture2D_path.string());
    if (iter != texturePathMap.end()) {
        iter->second.get()->refCount++;
        newTextureID = iter->second.get()->ID;
    }
    else {
        if (validateNextID() == false) {
            loggerPtr->addLog(LogLevel::LOG_ERROR, "createTexture2D", "could not validate nextMaterialInstanceID");
        }

        newTextureID = nextTextureID;
        auto textureInstancePtr = std::make_shared<TextureInstance>(std::make_unique<Texture2D>(texture2D_path.string()), 1, newTextureID);
        textureIDMap.emplace(newTextureID, textureInstancePtr);
        texturePathMap.emplace(texture2D_path.string(), textureInstancePtr);
        assignName(newTextureID, texture2D_path.filename().string());
        nextTextureID++;
    }
    return newTextureID;
}


unsigned int TextureCache::createCubeMap(std::vector<std::filesystem::path> texture_paths) {
    unsigned int newTextureID;
    if (validateNextID() == false) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "createTexture2D", "could not validate nextMaterialInstanceID");
        return std::numeric_limits<unsigned int>::max();
    }

    std::vector<std::string> paths_str(texture_paths.size());
    std::transform(texture_paths.begin(), texture_paths.end(), paths_str.begin(), 
        [](const std::filesystem::path& path) {
            return path.string();
        });

    newTextureID = nextTextureID;
    auto textureInstancePtr = std::make_shared<TextureInstance>(std::make_unique<CubeMap>(paths_str), 1, newTextureID);
    textureIDMap.emplace(newTextureID, textureInstancePtr);
    assignName(newTextureID, "cubemap");
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
    missingTexture2D->bind(texUnit);
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


std::vector<std::filesystem::path> TextureCache::getTexturePaths(unsigned int textureID) {
    if (textureIDMap.contains(textureID) == false) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "TEXTURECACHE | getTexturePath", "texture not found with ID " + std::to_string(textureID));
        std::vector<std::filesystem::path> emptypaths;
        return emptypaths;
    }

    std::vector<std::string> paths_str = textureIDMap.at(textureID)->texture->paths;
    std::vector<std::filesystem::path> texture_paths(paths_str.size());
    std::transform(paths_str.begin(), paths_str.end(), texture_paths.begin(), 
        [](const std::string& path) {
            return std::filesystem::path(path);
        }
    );

    return texture_paths;
}

unsigned int TextureCache::getTextureTexUnit(unsigned int textureID) {
    if (!textureIDMap.contains(textureID)) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "TEXTURECACHE | getTextureTexUnit", "texture not found with ID " + std::to_string(textureID));
        return 0;
    }

    return textureIDMap.at(textureID)->texture->getTexUnit();
}


std::string TextureCache::getName(unsigned int textureID) {
    if (!textureIDMap.contains(textureID)) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "TEXTURECACHE | getName", "texture not found with ID " + std::to_string(textureID));
        return "";
    }

    return textureIDMap.at(textureID)->texture->getName();
}


bool TextureCache::assignName(unsigned int textureID, std::string name) {
    bool success = false;
    if (!textureIDMap.contains(textureID)) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "TEXTURECACHE | assignName", "texture not found with ID " + std::to_string(textureID));
        return success;
    }

    int maxAttempts = 100;
    std::string namePostfix = "";
    int postfix = 1;
    do {
        if (!allTextureNames.contains(name + namePostfix)) {
            textureIDMap.at(textureID)->texture->setName(name + namePostfix);
            allTextureNames.emplace(name + namePostfix);
            success = true;
        }
        namePostfix = std::to_string(postfix++);
    }
    while (!success && postfix < maxAttempts);
    
    return success;
}