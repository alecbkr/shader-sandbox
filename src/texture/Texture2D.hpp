#pragma once

#include "Texture.hpp"
#include "TextureType.hpp"
#include <string>

class Texture2D : public Texture{
    public:
        Texture2D(std::string texture_path, TextureType type);
        ~Texture2D() = default;
        void bind(unsigned int texNum);

    private:
        void loadToGPU();
};