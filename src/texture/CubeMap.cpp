#include "CubeMap.hpp"

#include "../core/logging/Logger.hpp"
#include "core/logging/LogSink.hpp"
#include <stb_image.h>

CubeMap::CubeMap(std::string cubemap_dir) : Texture(cubemap_dir, TEX_CUBEMAP) {
    cubemap_paths = {
        cubemap_dir + "/right.jpg",
        cubemap_dir + "/left.jpg",
        cubemap_dir + "/top.jpg",
        cubemap_dir + "/bottom.jpg",
        cubemap_dir + "/front.jpg",
        cubemap_dir + "/back.jpg"
    };

    isInitialized = true;
}


void CubeMap::bind(unsigned int texNum) {
    loadToGPU();
    glActiveTexture(GL_TEXTURE0 + texNum);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
    this->texNum = texNum;
}


void CubeMap::loadToGPU() {
    if (isInitialized == false) {
        Logger::addLog(LogLevel::WARNING, "CUBEMAP", "Can't send uninitialized texture to GPU");
        return;
    } 
    if (isLoadedInGPU) return;

    Logger::addLog(LogLevel::INFO, "CUBEMAP", "Loading texture...");
    unsigned char* data;
    GLenum format;
    int width, height, channelCnt;

    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
    stbi_set_flip_vertically_on_load(false);
    
    for (unsigned int i = 0; i < cubemap_paths.size(); i++) {
        data = stbi_load(cubemap_paths[i].c_str(), &width, &height, &channelCnt, 0);
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
    isLoadedInGPU = true;
}