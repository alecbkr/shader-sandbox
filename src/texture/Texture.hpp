#pragma once

#include <glad/glad.h>
#include "platform/GL.hpp"
#include <string>
#include <vector>
#include "TextureStatus.hpp"

class Logger;


class Texture {
    
public:
    Texture(std::string texture_path);
    virtual ~Texture();
    virtual bool bind(unsigned int texUnit) = 0;
    void unbind();

    void setPath(std::string path);
    const std::string getPath() const;
    const unsigned int getTexUnit() const;
    TextureStatus getStatus() const;
    

protected:
    mutable GLuint gl_ID = 0;
    std::string path;
    unsigned int texUnit;
    TextureStatus status;

    virtual void loadToGPU() = 0;
    void unloadFromGPU();

private:
        
};
