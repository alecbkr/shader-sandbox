#pragma once

#include <string>
#include <vector>
#include "Texture.hpp"
#include "TextureType.hpp"
#include <glad/glad.h>

class CubeMap final : public Texture{
    public:
        std::vector<std::string> cubemap_paths;
        CubeMap(std::string cubemap_dir, Logger* _loggerPtr);
        ~CubeMap() = default;
        void bind(unsigned int texNum);

    private:
        void loadToGPU();
};