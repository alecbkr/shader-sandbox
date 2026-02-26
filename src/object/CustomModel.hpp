#pragma once
#include "Model.hpp"

class ShaderRegistry;
class TextureCache;
class Logger;

class CustomModel final : public Model {
    public:
        // Logger* loggerPtr = nullptr;
        // TextureCache* textureCachePtr = nullptr;
        CustomModel(const unsigned int ID, TextureCache* _textureCachePtr, Logger* _loggerPtr);
        ~CustomModel() = default;
        void setMesh(std::vector<float> vertices, std::vector<unsigned int> indices, 
                     bool hasPos, bool hasNorm, bool hasUV) override;
        void addTexture(std::string texture_path, TextureType type) override;
};