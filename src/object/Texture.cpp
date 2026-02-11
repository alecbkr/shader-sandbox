#include "Texture.hpp"
#include <stb/stb_image.h>
#include <string>
#include "core/logging/Logger.hpp"


Texture::Texture(const char *texture_path, TextureType type, Logger* _loggerPtr) : loggerPtr(_loggerPtr) {
    stbi_set_flip_vertically_on_load(true);

    int channelCnt;
    pixels = stbi_load(texture_path, &width, &height, &channelCnt, 0);
    if (pixels == nullptr) {
        if (loggerPtr) loggerPtr->addLog(LogLevel::LOG_ERROR, "TEXTURE", "Could not find texture from path:",  texture_path);
        valid = false;
        return;
    }

    switch (channelCnt) {
        case 1: format = GL_RED;  break;
        case 3: format = GL_RGB;  break;
        case 4: format = GL_RGBA; break;
        default: 
            if (loggerPtr) loggerPtr->addLog(LogLevel::LOG_ERROR, "TEXTURE", "Format could not be determined");
            valid = false;
            return;
    }
    this->type = type;
    valid = true;
}


// Texture::~Texture() {
//     unloadFromGPU();
//     stbi_image_free(pixels);
// }


void Texture::bind(int texNum) {
    loadToGPU();
    glActiveTexture(GL_TEXTURE0 + texNum);
    glBindTexture(GL_TEXTURE_2D, ID);
}


void Texture::unbind(int texNum) {
    glActiveTexture(GL_TEXTURE0 + texNum);
    glBindTexture(GL_TEXTURE_2D, 0);
}


bool Texture::isValid() const { 
    return valid;
}


void Texture::loadToGPU() {
    if (valid == false) {
        loggerPtr->addLog(LogLevel::WARNING, "TEXTURE", "Can't send uninitialized texture to GPU");
        return;
    } 

    if (isLoadedInGPU) return;

    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Texture Settings:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    isLoadedInGPU = true;
}


void Texture::unloadFromGPU() {
    if (isLoadedInGPU == false) return;
    glDeleteTextures(1, &ID);

    isLoadedInGPU = false;
}


TextureType Texture::getType() {
    return type;
}
