#pragma once

#include "CustomModel.hpp"
#include "ImportedModel.hpp"
#include <vector>


class ModelCache {
    public:
        ModelCache() = delete;

        static void createModel(const unsigned int ID, std::vector<float>, std::vector<unsigned int>, 
                                 bool hasPos, bool hasNorms, bool hasUVs);
        static void createModel(const unsigned int ID, std::string pathname);
        
        static void setTexture(const int ID, std::string pathname, std::string uniformName);
        static void setProgram(const int ID, ShaderProgram& program);

        static void translateModel(const int ID, glm::vec3 pos);
        static void scaleModel(const int ID, glm::vec3 scale);
        static void rotateModel(const int ID, float angle, glm::vec3 axis);
        
        static void renderModel(const int ID, glm::mat4 projection, glm::mat4 view);
        static void renderAll(glm::mat4 projection, glm::mat4 view);
        
        static std::unordered_map<unsigned int, std::unique_ptr<Model>> modelIDMap; 
        
        // DEBUG
        static void printOrder();

    private:
        static Model* getModel(unsigned int ID);
        static void reorderByProgram();
        static std::vector<std::unique_ptr<Model>> modelCache; 
        
};