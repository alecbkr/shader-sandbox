#pragma once
#include "Model.hpp"

class LoggerPtr;
class ShaderRegistry;

class ImportedModel final : public Model {
    public:
        ImportedModel(const unsigned int ID, std::string pathname, ShaderRegistry* _shaderRegPtr, Logger* _loggerPtr);
        ~ImportedModel() = default;
        unsigned int getID() const;
        void addMesh(std::vector<Vertex> vertices, std::vector<unsigned int>indices, MeshFlags meshflags);
        void addMaterial(MaterialProperties properties, std::vector<unsigned int> textureIDs, MaterialType type);
        void addTexture(std::string texture_path, TextureType type) override;

        std::vector<std::unique_ptr<MeshA>>& getMeshVec();
        std::vector<std::unique_ptr<Material>>& getMatVec();
        // std::unordered_map<std::string, std::shared_ptr<Texture>>& getTextures();
};