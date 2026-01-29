#pragma once
#include "Model.hpp"

class ImportedModel : public Model {
    public:
        ImportedModel(const unsigned int ID, std::string pathname);

        std::vector<MeshA>& getMeshes();
        std::unordered_map<std::string, std::shared_ptr<Texture>>& getTextures();
};