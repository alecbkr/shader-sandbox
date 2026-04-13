#pragma once

#include <string>
#include <vector>
#include "Texture.hpp"
#include "TextureType.hpp"
#include <glad/glad.h>

class CubeMap final : public Texture{
    public:
        CubeMap(std::vector<std::string> cubemap_paths);
        ~CubeMap() = default;
        bool bind(unsigned int texUnit) override;

    private:
        void loadToGPU() override;
};