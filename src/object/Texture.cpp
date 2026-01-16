#include "Texture.hpp"
#include <stb/stb_image.h>
#include <string>
#include "core/logging/LogSink.hpp"
#include "core/logging/Logger.hpp"


Texture::Texture(const char *texture_path) {
    if (!texture_path  || texture_path[0] == '\0') {
        Logger::addLog(LogLevel::WARNING, "Texture::Texture", "no texture path given!");
        return;
    }
    this->path = texture_path; 
    stbi_set_flip_vertically_on_load(true);

    int channelCnt;
    pixels = stbi_load(texture_path, &width, &height, &channelCnt, 0);
    if (!(pixels == nullptr)) {
        
        switch (channelCnt) {
            case 1: format = GL_RED;  break;
            case 3: format = GL_RGB;  break;
            case 4: format = GL_RGBA; break;
            default: 
                Logger::addLog(LogLevel::ERROR, "TEXTURE", "Format could not be determined");
                return;
        }

        initialized = true;
    }
    else {
        Logger::addLog(LogLevel::ERROR, "TEXTURE", "Could not find texture from path:", texture_path);
    }
}


Texture::~Texture() {
    deleteFromGPU();
    stbi_image_free(pixels);
}


void Texture::bind(int texNum) const {
    sendToGPU();
    glActiveTexture(GL_TEXTURE0 + texNum);
    glBindTexture(GL_TEXTURE_2D, ID);
}


void Texture::unbind(int texNum) {
    glActiveTexture(GL_TEXTURE0 + texNum);
    glBindTexture(GL_TEXTURE_2D, 0);
}


bool Texture::isValid() const { 
    return initialized;
}


void Texture::sendToGPU() const {
    if (!initialized) {
        Logger::addLog(LogLevel::ERROR, "TEXTURE", "Can't send uninitialized texture to GPU");
        return;
    } 
    if (!pixels) {
        Logger::addLog(LogLevel::ERROR, "TEXTURE", "pixel memory is invalid!");
        return;
    } 

    if (loadedInGPU) return;

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
    loadedInGPU = true;
}


void Texture::deleteFromGPU() {
    if (!loadedInGPU) return;
    glDeleteTextures(1, &ID);

    loadedInGPU = false;
}
