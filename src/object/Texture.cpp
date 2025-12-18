#include "Texture.hpp"
#include <stb/stb_image.h>
#include "engine/Errorlog.hpp"


Texture::Texture(const char *texture_path) {
    
    stbi_set_flip_vertically_on_load(true);

    int channelCnt;
    pixels = stbi_load(texture_path, &width, &height, &channelCnt, 0);
    if (!(pixels == nullptr)) {
        
        switch (channelCnt) {
            case 1: format = GL_RED;  break;
            case 3: format = GL_RGB;  break;
            case 4: format = GL_RGBA; break;
            default: 
                ERRLOG.logEntry(EL_ERROR, "TEXTURE", "Format could not be determined");
                return;
        }

        initialized = true;
    }
    else {
        ERRLOG.logEntry(EL_ERROR, "TEXTURE", "Could not find texture from path:", texture_path);
    }
}


Texture::~Texture() {
    deleteFromGPU();
    stbi_image_free(pixels);
}


void Texture::bind(int texNum) {
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


void Texture::sendToGPU() {
    if (!initialized) {
        ERRLOG.logEntry(EL_WARNING, "TEXTURE", "Can't send uninitialized texture to GPU");
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