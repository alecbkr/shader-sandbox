#include "Texture2D.hpp"
#include "../core/logging/Logger.hpp"
#include "../core/logging/LogSink.hpp"
#include <stb_image.h>

Texture2D::Texture2D(std::string texture_path, TextureType type) : Texture(texture_path, type) {

}


void Texture2D::bind(unsigned int texNum) {
    loadToGPU();
    glActiveTexture(GL_TEXTURE0 + texNum);
    glBindTexture(GL_TEXTURE_2D, ID);
    this->texNum = texNum;
}


void Texture2D::loadToGPU() {
    if (isInitialized == false) {
        Logger::addLog(LogLevel::WARNING, "TEXTURE", "Can't send uninitialized texture to GPU");
        return;
    } 
    if (isLoadedInGPU) return;

    stbi_set_flip_vertically_on_load(true);
    unsigned char* data;
    GLenum format;
    int width, height, channelCnt;
    data = stbi_load(path.c_str(), &width, &height, &channelCnt, 0);
    if (data == nullptr) {
        Logger::addLog(LogLevel::ERROR, "TEXTURE", "Could not find texture from path:",  path);
        valid = false;
        return;
    }

    switch (channelCnt) {
        case 1: format = GL_RED;  break;
        case 3: format = GL_RGB;  break;
        case 4: format = GL_RGBA; break;
        default: 
            Logger::addLog(LogLevel::ERROR, "TEXTURE", "Format could not be determined");
            valid = false;
            return;
    }

    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Texture Settings:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    isLoadedInGPU = true;
}


void Texture2D::unloadFromGPU() {
    if (isLoadedInGPU == false) return;
    glDeleteTextures(1, &ID);

    isLoadedInGPU = false;
}