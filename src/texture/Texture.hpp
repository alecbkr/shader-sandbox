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

// #include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "TextureType.hpp"
#include <string>
#include <vector>


class Texture {
    
    public:
        mutable GLuint ID = 0;
        std::string path;
        
        Texture(std::string texture_path, TextureType type);
        virtual ~Texture();
        virtual void bind(unsigned int texNum) = 0;
        void unbind();
        void unloadFromGPU();
        bool isValid() const;
        TextureType getType();

    protected:
        TextureType type = TEX_UNDEFINED;
        unsigned int texNum;
        bool valid = false;
        bool isInitialized = false;
        bool isLoadedInGPU = false;
    
    private:
        
};

#endif
