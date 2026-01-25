#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>

#include "../engine/ShaderProgram.hpp"
#include "MeshAssimp.hpp"
#include "Texture.hpp"

class Model {
    private:
        struct ModelFlags {
            bool hasMeshes = false;
            bool hasTextures = false;
            bool hasProgram = false;
        };

    public:
        const int ID;
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 scale = glm::vec3(1.0f);
        glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        glm::mat4 modelM = glm::mat4(1.0f);
        
        Model(const unsigned int ID);
        // FUNCTIONALITY
        void renderModel();
        void unloadModel();

        // TRANSLATIONS
        void translate(glm::vec3 position);
        void rescale(glm::vec3 vector);
        void rotate(float angle, glm::vec3 axis);

        // SETTERS
        void setProgram(ShaderProgram &program);
        virtual void setMesh(std::vector<float> vertices, std::vector<unsigned int> indices, bool hasPos, bool hasNorm, bool hasUV);
        virtual void addTexture(std::string pathname);

        // GETTERS
        ShaderProgram* getProgram();
        const int getID();
        

    protected:
        ModelFlags properties;
        std::vector<MeshA> all_meshes;
        std::unordered_map<std::string, std::shared_ptr<Texture>> all_textures;

    private:
        ShaderProgram *program = nullptr;
        glm::mat4 calcModelM();
        void bindTextures(MeshA& mesh);
};