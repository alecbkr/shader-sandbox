#pragma once
#include "Model.hpp"

class LoggerPtr;
class ShaderRegistry;
class ModelImporter;

class ImportedModel : public Model {
    public:
        ImportedModel() = delete;
        ImportedModel(const unsigned int ID, std::string pathname, ModelImporter* _modelImporter, ShaderRegistry* _shaderRegPtr, Logger* _loggerPtr);

        std::vector<MeshA>& getMeshes();
        std::unordered_map<std::string, std::shared_ptr<Texture>>& getTextures();
};