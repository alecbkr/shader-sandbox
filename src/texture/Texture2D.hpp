#pragma once

#include "Texture.hpp"
#include "TextureType.hpp"
#include <string>

class Texture2D : public Texture{
    public:
        Texture2D(std::string texture_path);
        ~Texture2D() = default;
        bool bind(unsigned int texUnit) override;

    private:
        void loadToGPU() override;
};