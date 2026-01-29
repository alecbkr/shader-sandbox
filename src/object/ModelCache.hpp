#pragma once

#include "CustomModel.hpp"
#include "ImportedModel.hpp"
#include <vector>

static constexpr unsigned int INVALID_MODEL = UINT_MAX;

class ModelCache {
    public:
        static bool initialize();
        ModelCache() = delete;

        static unsigned int createModel(std::vector<float>, std::vector<unsigned int>, 
                                                        bool hasPos, bool hasNorms, bool hasUVs);
        static unsigned int createModel(std::string pathname);
        
        static void setTexture(unsigned int ID, std::string pathname, std::string uniformName);
        static void setProgram(unsigned int ID, ShaderProgram& program);

        static void translateModel(unsigned int ID, glm::vec3 pos);
        static void scaleModel(unsigned int ID, glm::vec3 scale);
        static void rotateModel(unsigned int ID, float angle, glm::vec3 axis);
        
        static void renderModel(unsigned int ID, glm::mat4 projection, glm::mat4 view);
        static void renderAll(glm::mat4 projection, glm::mat4 view);

        static int getNumberOfModels();
        
        static std::unordered_map<unsigned int, std::unique_ptr<Model>> modelIDMap; 
        
        static Model* getModel(unsigned int ID);

        // DEBUG
        static void printOrder();

    private:
        static unsigned int nextModelID;
        static void reorderByProgram();
        static std::vector<std::unique_ptr<Model>> modelCache; 
        
};