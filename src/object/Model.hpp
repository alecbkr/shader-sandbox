#pragma once

#include <vector>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "MeshAssimp.hpp"
#include "ModelStatus.hpp"
#include "ModelType.hpp"

struct MeshInstance {
    unsigned int meshIdx;
    unsigned int materialID;
};

class Model {
public:
    const unsigned int ID;
    const std::string model_path;
    const ModelType type;
    
    Model(const unsigned int ID, std::string model_path, ModelType type);
    virtual ~Model() = default;

    void drawMesh(unsigned int meshIdx);
    bool addMeshByData(std::vector<float> raw_vertices, std::vector<unsigned int> indices, bool hasPos, bool hasNorm, bool hasUV);
    void addMeshByAssimp(std::vector<Vertex> vertices, std::vector<unsigned int> indices, bool hasPos, bool hasNorms, bool hasUVs);
    //unloadMesh

    void translate(glm::vec3 position);
    void rescale(glm::vec3 vector);
    void rotate(float angle, glm::vec3 axis);

    void setPosition(glm::vec3 position);
    void setScale(glm::vec3 scale);
    void setRotation(float angle, glm::vec3 axis);
    void setInstancePosition(unsigned int instanceNum, glm::vec3 position);
    void setInstanceCount(unsigned int newInstanceCount);
    void setName(std::string name);
    void setMeshMaterial(unsigned int meshIdx, unsigned int materialID);
    void setModelMaterial(unsigned int materialID);

    // GETTERS
    std::string getName() const;
    unsigned int getID() const;
    std::string getPath() const;
    ModelStatus& getModelStatus();
    glm::mat4 getModelMatrix() const;
    glm::vec3 getPosition() const;
    glm::vec3 getScale() const;
    glm::vec4 getRotation() const;
    unsigned int getInstanceCount() const;
    const std::vector<InstanceData>& getInstanceData() const;
    const std::vector<MeshInstance>& getMeshInstances() const;
    const std::unordered_map<unsigned int, unsigned int>& getAllMaterialReferences() const;

    //LOADING
    void loadInstanceData(std::vector<InstanceData> instanceData);
    void loadMeshMaterialIDs(std::vector<unsigned int> meshMaterialIDs);
    std::vector<unsigned int> getAllMaterialIDsPerMesh();
    bool finalizeMeshes();
    void setMaterialStateReady();

    //DEBUG
    unsigned int getNumberOfMeshes();
    static std::string getProgramID(); //DUMMY FUNCTION, ALL CALLS SHOULD BE CHANGED TO GRAB FROM MATERIAL
    static void setProgramID(std::string programID); //ALSO DUMMY

private:
    std::string name = "model";
    unsigned int nextMeshIdx = 0;
    std::vector<MeshA> meshes;
    std::unordered_map<unsigned int, unsigned int> allMaterialReferences; //[material id] <-> [# of meshes using material]
    std::vector<MeshInstance> meshInstances;
    unsigned int modelInstanceCount = 1;
    std::vector<InstanceData> instanceData;

    ModelStatus status;
    glm::mat4 modelM      = glm::mat4(1.0f);
    glm::vec3 position    = glm::vec3(0.0f);
    glm::vec3 scale       = glm::vec3(1.0f);
    glm::vec4 rotation    = glm::vec4(0.0f); // raw values for orientation calc
    glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    void calcModelM();
};
