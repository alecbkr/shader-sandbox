#pragma once
#include "Model.hpp"

class CustomModel : public Model {
    public:
        MeshA* meshptr;
        CustomModel(const unsigned int ID);
        void setMesh(std::vector<float> vertices, std::vector<unsigned int> indices, 
                     bool hasPos, bool hasNorm, bool hasUV) override;
        void addTexture(std::string filepath) override;
};