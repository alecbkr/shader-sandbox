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
        const unsigned int ID;
        std::string name; // PLACEHOLDER does not do anything rn
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

        void setPosition(glm::vec3 position);
        void setScale(glm::vec3 scale);
        void setRotation(float angle, glm::vec3 axis);

        // GETTERS
        ShaderProgram* getProgram();
        const int getID();

        glm::vec3 getPosition();
        glm::vec3 getScale();
        glm::vec4 getRotation();
        

    protected:
        ModelFlags properties;
        std::vector<MeshA> all_meshes;
        std::unordered_map<std::string, std::shared_ptr<Texture>> all_textures;

    private:
        glm::vec3 position    = glm::vec3(0.0f);
        glm::vec3 scale       = glm::vec3(1.0f);
        glm::vec4 rotation    = glm::vec4(0.0f); // raw values for orientation calc
        glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        ShaderProgram *program = nullptr;
        void calcModelM();
        void bindTextures(MeshA& mesh);
};