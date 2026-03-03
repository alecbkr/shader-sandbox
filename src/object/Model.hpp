#pragma once

#include <vector>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../engine/ShaderProgram.hpp"
#include "MeshAssimp.hpp"
#include "../texture/Texture.hpp"
#include "Material.hpp"
#include "ModelPrimitive.hpp"
#include "object/MaterialCache.hpp"

class ShaderRegistry;
class TextureCache;
class Logger;
class MaterialCache;

class Model {
    private:
        struct ModelFlags {
            bool hasMeshes = false;
            bool hasTextures = false;
            bool hasProgram = false;
        };

    public:
        const unsigned int ID;
        std::string        name; // PLACEHOLDER does not do anything rn
        std::vector<ModelPrimitive> primitives;

        // SYSTEM POINTERS
        ShaderRegistry* shaderRegPtr = nullptr;
        TextureCache* textureCachePtr = nullptr;
        Logger* loggerPtr = nullptr;
        MaterialCache* materialCachePtr = nullptr;

        Model(const unsigned int ID, TextureCache* _textureCachePtr, Logger* _loggerPtr, MaterialCache* _materialCachePtr);
        virtual ~Model() = default;
        // FUNCTIONALITY
        void renderPrimitive(unsigned int meshID);
        void unloadAllPrimitives();

        // TRANSLATIONS
        void setPosition(glm::vec3 position);
        void setScale(glm::vec3 scale);
        void setRotation(float angle, glm::vec3 axis);
        void translate(glm::vec3 position);
        void rescale(glm::vec3 vector);
        void rotate(float angle, glm::vec3 axis);

        // SETTERS
        virtual void setMesh(std::vector<float> vertices, std::vector<unsigned int> indices, bool hasPos, bool hasNorm, bool hasUV);
        void setModelProgram(std::string &programID); // set shader for all materials in model
        virtual void addTexture(std::string texture_path, TextureType type) = 0;
        void setMaterialProgram(unsigned int materialID, std::string &programID);
        void setMaterialType(unsigned int materialID, MaterialType type);

        // GETTERS
        const std::vector<unsigned int>& getAllMaterialIDs() const;
        std::string getMaterialProgramID(unsigned int materialID) const;
        MaterialType getMaterialType(unsigned int materialID) const;
        glm::mat4 getModelMatrix() const;
        glm::vec3 getPosition() const;
        glm::vec3 getScale() const;
        glm::vec4 getRotation() const;

        //DEBUG
        static std::string getProgramID(); //DUMMY FUNCTION, ALL CALLS SHOULD BE CHANGED TO GRAB FROM MATERIAL
        static void setProgramID(std::string programID); //ALSO DUMMY

    protected:
        unsigned int nextMeshID = 0;
        ModelFlags properties;
        std::vector<std::unique_ptr<MeshA>>    all_meshes;
        std::vector<unsigned int> all_material_ids;

    private:
        glm::mat4 modelM      = glm::mat4(1.0f);
        glm::vec3 position    = glm::vec3(0.0f);
        glm::vec3 scale       = glm::vec3(1.0f);
        glm::vec4 rotation    = glm::vec4(0.0f); // raw values for orientation calc
        glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        std::string programID; 
        void calcModelM();

        
};
