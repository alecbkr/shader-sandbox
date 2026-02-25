#include "Texture.hpp"
#include <stb/stb_image.h>
#include <string>
#include "core/logging/LogSink.hpp"
#include "core/logging/Logger.hpp"


Texture::Texture(std::string texture_path, TextureType type) {
    this->path = texture_path;
    this->type = type;
    
    isInitialized = true;
}



Texture::~Texture() {
    unloadFromGPU();
}




void Texture::unbind() {
    glActiveTexture(GL_TEXTURE0 + texNum);
    glBindTexture(GL_TEXTURE_2D, 0);
}


void Texture::unloadFromGPU() {
    if (isLoadedInGPU == false) return;
    glDeleteTextures(1, &ID);
    ID = 0;

    isLoadedInGPU = false;
}


bool Texture::isValid() const { 
    return valid;
}


TextureType Texture::getType() {
    return type;
}
