#include "Texture2D.hpp"
#include "../core/logging/Logger.hpp"
#include "../core/logging/LogSink.hpp"
#include <stb_image.h>


Texture2D::Texture2D(std::string texture_path) : Texture(texture_path) {
    status = TextureStatus::Ready;
}


bool Texture2D::bind(unsigned int texUnit) {
    loadToGPU();
    if (status != TextureStatus::Loaded) return false;

    glActiveTexture(GL_TEXTURE0 + texUnit);
    glBindTexture(GL_TEXTURE_2D, gl_ID);
    this->texUnit = texUnit;
    return true;
}


void Texture2D::loadToGPU() {
    if (status == TextureStatus::Loaded) return;

    stbi_set_flip_vertically_on_load(true);
    unsigned char* data;
    GLenum format;
    int width, height, channelCnt;
    data = stbi_load(paths[0].c_str(), &width, &height, &channelCnt, 0);
    if (data == nullptr) {
        status = TextureStatus::FileNotFound;
        return;
    }

    switch (channelCnt) {
        case 1: format = GL_RED;  break;
        case 3: format = GL_RGB;  break;
        case 4: format = GL_RGBA; break;
        default: 
            status = TextureStatus::InvalidFormat;
            return;
    }

    glGenTextures(1, &gl_ID);
    glBindTexture(GL_TEXTURE_2D, gl_ID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Texture Settings:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0); //unbind
    status = TextureStatus::Loaded;

    return;
}