// DESCRIPTION ***OUTDATED***
/*
Texture constructor will save all the necessary data for OpenGL texture creation
and hold on to it until Destructor is ran. An OpenGL texture will NOT be produced
until sendToGPU() is ran. Binding will handle this automatically. If texture was
not set up properly, "initialized" will be false, and functions will halt if called.

Honestly, it might make more sense to include this in the engine dir, but whatever here it
is for now.
*/

#pragma once


// #include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "platform/GL.hpp"
#include "TextureType.hpp"
#include <string>
#include <vector>

class Logger;

class Texture {
    
    public:
        mutable GLuint ID = 0;
        std::string path;
        
        Logger* loggerPtr;
        Texture(std::string texture_path, TextureType type, Logger* _loggerPtr);
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
