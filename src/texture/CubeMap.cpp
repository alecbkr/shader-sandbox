#include "CubeMap.hpp"

#include "../core/logging/Logger.hpp"
#include "core/logging/LogSink.hpp"
#include <stb_image.h>

CubeMap::CubeMap(std::vector<std::string> cubemap_paths) : Texture(cubemap_paths) {
    status = TextureStatus::Ready;
}


bool CubeMap::bind(unsigned int texUnit) {
    loadToGPU();
    if (status != TextureStatus::Loaded) return false;
    glActiveTexture(GL_TEXTURE0 + texUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, gl_ID);
    this->texUnit = texUnit;
    return true;
}


void CubeMap::loadToGPU() {
    if (status == TextureStatus::Loaded) return;

    unsigned char* data;
    GLenum format;
    int width, height, channelCnt;

    glGenTextures(1, &gl_ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, gl_ID);
    stbi_set_flip_vertically_on_load(false);
    
    for (unsigned int i = 0; i < paths.size(); i++) {
        data = stbi_load(paths[i].c_str(), &width, &height, &channelCnt, 0);
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

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
        // glGenerateMipmap(GL_TEXTURE_2D);
    }

    // Texture Settings:
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    status = TextureStatus::Loaded;
}