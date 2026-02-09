#pragma once

#include "Texture.hpp"
#include "TextureType.hpp"
#include <string>

class Texture2D : public Texture{
    public:
        Texture2D(std::string texture_path, TextureType type);
        void bind(unsigned int texNum);

    private:
        void loadToGPU();
        void unloadFromGPU();
};