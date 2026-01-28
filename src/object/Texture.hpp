// DESCRIPTION ***OUTDATED***
/*
Texture constructor will save all the necessary data for OpenGL texture creation
and hold on to it until Destructor is ran. An OpenGL texture will NOT be produced
until sendToGPU() is ran. Binding will handle this automatically. If texture was
not set up properly, "initialized" will be false, and functions will halt if called.

Honestly, it might make more sense to include this in the engine dir, but whatever here it
is for now.
*/


#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <glad/glad.h>
#include "TextureType.hpp"
#include <string>


class Texture {
    
    public:
        mutable GLuint ID;
        std::string path;
        unsigned char* pixels;
        
        int width;
        int height;
        GLenum format;
        Texture(const char *texture_path, TextureType type);
        // ~Texture();
        void bind(int texNum);
        void unbind(int texNum);
        bool isValid() const;
        TextureType getType();
    
    private:
        TextureType type = TEX_UNDEFINED;
        bool valid = false;
        bool isLoadedInGPU = false;
        void loadToGPU();
        void unloadFromGPU();
};

#endif
