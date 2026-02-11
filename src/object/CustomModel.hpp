#pragma once
#include "Model.hpp"

class ShaderRegistry;
class Logger;

class CustomModel : public Model {
    public:
        MeshA* meshptr;
        Logger* loggerPtr = nullptr;
        CustomModel(const unsigned int ID, ShaderRegistry* shaderRegPtr, Logger* _loggerPtr);
        void setMesh(std::vector<float> vertices, std::vector<unsigned int> indices, 
                     bool hasPos, bool hasNorm, bool hasUV) override;
        void addTexture(std::string filepath) override;
};