#pragma once
#include "Model.hpp"

class CustomModel final : public Model {
    public:
        CustomModel(const unsigned int ID);
        ~CustomModel() = default;
        void setMesh(std::vector<float> vertices, std::vector<unsigned int> indices, 
                     bool hasPos, bool hasNorm, bool hasUV) override;
        void addTexture(std::string texture_path, TextureType type) override;
};