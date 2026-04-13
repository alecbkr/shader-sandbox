#pragma once

#include <glad/glad.h>
#include "platform/GL.hpp"
#include <string>
#include <vector>
#include "TextureStatus.hpp"

class Logger;


class Texture {
public:
    const std::vector<std::string> paths;
    Texture(std::string texture_path);
    Texture(std::vector<std::string> texture_paths);
    virtual ~Texture();
    virtual bool bind(unsigned int texUnit) = 0;
    void unbind();

    // void setPath(std::string path);
    // const std::string getPath() const;
    const unsigned int getTexUnit() const;
    TextureStatus getStatus() const;
    

protected:
    mutable GLuint gl_ID = 0;
    TextureStatus status;
    unsigned int texUnit;
    
    virtual void loadToGPU() = 0;
    void unloadFromGPU();  
};
