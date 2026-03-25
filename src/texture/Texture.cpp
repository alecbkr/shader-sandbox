#include "Texture.hpp"
#include <stb/stb_image.h>
#include <string>
#include "core/logging/LogSink.hpp"
#include "core/logging/Logger.hpp"


Texture::Texture(std::string texture_path) : path(texture_path) {

}


Texture::~Texture() {
    unloadFromGPU();
}


void Texture::unbind() {
    unloadFromGPU();
    glActiveTexture(GL_TEXTURE0 + texUnit);
    glBindTexture(GL_TEXTURE_2D, 0);
}


TextureStatus Texture::getStatus() const {
    return status;
}


void Texture::setPath(std::string newPath) {
    if (path == newPath) return;
    
    unloadFromGPU();
    this->path = newPath;
    status = TextureStatus::Ready;
}


const std::string Texture::getPath() const {
    return path;
}


void Texture::unloadFromGPU() {
    if (status == TextureStatus::Ready) return;
    glDeleteTextures(1, &gl_ID);
    gl_ID = 0;

    status = TextureStatus::Ready;
}
