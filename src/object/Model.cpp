#include "Model.hpp"

#include <iostream>


Model::Model(const unsigned int ID, std::string model_path, ModelType type)
    : ID(ID), model_path(model_path), type(type) {
        instanceData.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f)); //instace #1 value
    }


void Model::drawMesh(unsigned int meshID) {
    if (meshID >= meshes.size()) return;
    MeshA* mesh = &meshes[meshID];
    // mesh->setInstanceVBO(modelInstanceCount, instanceData);
    mesh->bind(instanceData);

    if (modelInstanceCount > 1) {
        glDrawElementsInstanced(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0, modelInstanceCount);
    }
    else {
        glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
    }
}


bool Model::addMeshByData(std::vector<float> raw_vertices, std::vector<unsigned int> indices, bool hasPos, bool hasNorm, bool hasUV) {
    // meshes->unloadFromGPU();

    if (status.meshes == ModelState::Error || status.meshes == ModelState::Ready) return false;

    unsigned int rowstride = 0;
    rowstride += 3*hasPos;
    rowstride += 3*hasNorm;
    rowstride += 2*hasUV;

    if (raw_vertices.size() % rowstride != 0) {
        status.meshes = ModelState::Error;
        return false;
    }

    // VERTICES
    std::vector<Vertex> vertices;
    for (unsigned int row = 0; row < raw_vertices.size() / rowstride; row++) {
        Vertex vertex;
        unsigned int col = 0;

        if (hasPos) {
            glm::vec3 vector3;
            vector3.x = raw_vertices.at((col++) + (row*rowstride));
            vector3.y = raw_vertices.at((col++) + (row*rowstride));
            vector3.z = raw_vertices.at((col++) + (row*rowstride));
            vertex.position = vector3;
        }

        if (hasNorm) {
            glm::vec3 vector3;
            vector3.x = raw_vertices.at((col++) + (row*rowstride));
            vector3.y = raw_vertices.at((col++) + (row*rowstride));
            vector3.z = raw_vertices.at((col++) + (row*rowstride));
            vertex.normal = vector3;
        }

        if (hasUV) {
            glm::vec2 vector2;
            vector2.x = raw_vertices.at((col++) + (row*rowstride));
            vector2.y = raw_vertices.at((col++) + (row*rowstride));
            vertex.uv = vector2;
        }

        vertices.push_back(vertex);
    }

    meshes.emplace_back(MeshA(nextMeshIdx, vertices, indices, hasPos, hasNorm, hasUV));
    meshInstances.emplace_back(nextMeshIdx, UINT_MAX);

    status.meshes = ModelState::Building;
    nextMeshIdx++;
    return true;
}


void Model::addMeshByAssimp(std::vector<Vertex> vertices, std::vector<unsigned int> indices, bool hasPos, bool hasNorms, bool hasUVs) {
    meshes.push_back(MeshA(nextMeshIdx, vertices, indices, hasPos, hasNorms, hasUVs));
    meshInstances.emplace_back(nextMeshIdx, UINT_MAX);
    
    // if (allMaterialReferences.contains(0)) {
    //     allMaterialReferences.at(0)++;
    // }
    // else {
    //     allMaterialReferences.emplace(0, 1);
    // }

    status.meshes = ModelState::Building;
    nextMeshIdx++;
}


// -----TRANSLATIONS
void Model::translate(glm::vec3 vector) {
    position += vector;
    calcModelM();
}


void Model::rescale(glm::vec3 vector) {
    scale *= vector;
    calcModelM();
}


// void Model::rotate(float angle, glm::vec3 axis) {
//     float processedAngle = fmod((rotation.x + angle), 360.0f);
//     if (processedAngle < 0) processedAngle += 360.0f;

//     glm::vec3 processedAxis = glm::clamp(glm::vec3(rotation.y+axis.x, rotation.z+axis.y, rotation.w+axis.z), -1.0f, 1.0f);
//     glm::vec3 normalizedAxis = processedAxis;
//     if (glm::length(normalizedAxis) > 0.0001f) {
//         normalizedAxis = glm::normalize(normalizedAxis);
//     }

//     rotation = glm::vec4(processedAngle, processedAxis);
//     this->orientation = glm::angleAxis(glm::radians(processedAngle), normalizedAxis);
//     calcModelM();
// }


void Model::setPosition(glm::vec3 position) {
    this->position = position;
    calcModelM();
}


void Model::setScale(glm::vec3 scale) {
    this->scale = scale;
    calcModelM();
}


void Model::setRotation(float angle, glm::vec3 axis) {
    float processedAngle = fmod(angle, 360.0f);
    if (processedAngle < 0) processedAngle += 360.0f;

    glm::vec3 processedAxis = glm::clamp(axis, glm::vec3(-1.0f), glm::vec3(1.0f));
    glm::vec3 normalizedAxis = processedAxis;
    if (glm::length(normalizedAxis) > 0.0001f) {
        normalizedAxis = glm::normalize(normalizedAxis);
    }

    rotation = glm::vec4(processedAngle, processedAxis);
    this->orientation = glm::angleAxis(glm::radians(processedAngle), normalizedAxis);
    calcModelM();
}


void Model::setRotation(glm::vec3 rotation) {
    
    this->rotation = glm::mod(rotation, 360.0f);
    this->orientation = glm::quat(glm::radians(this->rotation));
    calcModelM();
}


void Model::setInstancePosition(unsigned int instanceNum, glm::vec3 position) {
    if (instanceNum > modelInstanceCount) return;
    
    instanceData[instanceNum].pos = position;
    for (MeshA& mesh : meshes) {
        mesh.updateInstanceData(instanceData);
    }
}


// -----SETTERS
void Model::setInstanceCount(unsigned int newInstanceCount) { 

    if (newInstanceCount < 1) {
        newInstanceCount = 1;
    } else if (newInstanceCount > 100) {
        newInstanceCount = 100;
    }

    if (newInstanceCount == modelInstanceCount) return; 

    if (newInstanceCount > modelInstanceCount) {
        for (int i = modelInstanceCount; i < newInstanceCount; i++) {
            instanceData.emplace_back(glm::vec3(1.0f * i, 0.0f, 0.0f));
        }
    }
    else { //new count is less than model
        for (int i = modelInstanceCount - 1; i > newInstanceCount; i--) {
            instanceData.erase(instanceData.begin() + i);
        }
    }
    modelInstanceCount = newInstanceCount;
    for (MeshA& mesh : meshes) {
        mesh.resizeInstanceVBO(instanceData);
    }
}


void Model::setName(std::string name) {
    this->name = name;
}


void Model::setMeshMaterial(unsigned int meshIdx, unsigned int materialID, bool isMatValid) {
    if (meshIdx >= meshInstances.size()) {
        return;
    }

    MeshInstance& meshInstance = meshInstances[meshIdx];

    // Remove old real material reference
    if (meshInstance.materialID != UINT_MAX) {
        auto it = allMaterialReferences.find(meshInstance.materialID);
        if (it != allMaterialReferences.end()) {
            if (--(it->second) == 0) {
                allMaterialReferences.erase(it);
                invalidMaterialIDs.erase(it->first);
            }
        }
    }

    // Assign new material
    meshInstance.materialID = materialID;
    if (isMatValid == false) {
        invalidMaterialIDs.insert(materialID);
    }

    // Only track real materials
    if (materialID != UINT_MAX) {
        allMaterialReferences[materialID]++;
    }

    // Model is renderable only if every mesh has a real material
    bool allMeshesHaveMaterial = true;
    for (const MeshInstance& mi : meshInstances) {
        if (mi.materialID == UINT_MAX) {
            allMeshesHaveMaterial = false;
            break;
        }
    }
    if (invalidMaterialIDs.empty() == false) status.material = ModelState::Invalid;
    else status.material = allMeshesHaveMaterial ? ModelState::Ready : ModelState::Building;
}


void Model::setModelMaterial(unsigned int materialID, bool isMatValid) {
    allMaterialReferences.clear();

    for (MeshInstance& meshInstance : meshInstances) {
        meshInstance.materialID = materialID;
    }

    if (materialID != UINT_MAX) {
        allMaterialReferences.emplace(materialID, static_cast<unsigned int>(meshInstances.size()));
        if (isMatValid == false) {
            invalidMaterialIDs.insert(materialID);
            status.material = ModelState::Invalid;
        }
        else {
            status.material = ModelState::Ready;
        }
        
    } else {
        status.material = ModelState::Building;
    }
}


bool Model::finalizeMeshes() {
    if (status.meshes != ModelState::Building) {
        return false;
    }
    status.meshes = ModelState::Ready;
    return true;
}


void Model::setMaterialStateReady() {
    status.material = ModelState::Ready;
}


void Model::calcModelM() {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model *= glm::mat4_cast(orientation); 
    model = glm::scale(model, scale);
    this->modelM = model;
}

void Model::loadInstanceData(std::vector<InstanceData> data) {
    instanceData = std::move(data);
    modelInstanceCount = static_cast<unsigned int>(instanceData.size());

    if (modelInstanceCount < 1) {
        instanceData.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f));
        modelInstanceCount = 1;
    }

    for (MeshA& mesh : meshes) {
        mesh.resizeInstanceVBO(instanceData);
    }
}


void Model::loadMeshMaterialIDs(std::vector<unsigned int> meshMaterialIDs) {
    for (unsigned int meshIdx = 0; meshIdx < meshInstances.size(); meshIdx++) {
        meshInstances[meshIdx].materialID = meshMaterialIDs[meshIdx];
    }

    rebuildMaterialReferences();
}


std::string Model::getName() const { return name; }
unsigned int Model::getID() const { return ID; }
std::string Model::getPath() const { return model_path; }
ModelStatus& Model::getModelStatus() { return status; }
glm::mat4 Model::getModelMatrix() const {return modelM;}
glm::vec3 Model::getPosition()    const {return position;}
glm::vec3 Model::getScale()       const {return scale;}
glm::vec3 Model::getRotation()    const {return rotation;}
unsigned int Model::getInstanceCount() const { return modelInstanceCount; }
const std::vector<InstanceData>& Model::getInstanceData() const { return instanceData; }
const std::vector<MeshInstance>& Model::getMeshInstances() const { return meshInstances; }
std::unordered_set<unsigned int>& Model::getInvalidMaterialIDs() { return invalidMaterialIDs; }
const std::unordered_map<unsigned int, unsigned int>& Model::getAllMaterialReferences() const { return allMaterialReferences; }


unsigned int Model::getNumberOfMeshes() { return meshes.size(); }

std::vector<unsigned int> Model::getAllMaterialIDsPerMesh() {

    std::vector<unsigned int> allMaterialIDsPerMesh;
    for (MeshInstance& meshInstance : meshInstances) {
        allMaterialIDsPerMesh.push_back(meshInstance.materialID);
    }
    return allMaterialIDsPerMesh;
}


std::string Model::getProgramID() {
    // Logger::addLog(LogLevel::ERROR, "MODEL-getProgramID()", "outdated getProgramID call, returning dummy value");
    return "";
}


void Model::setProgramID(std::string programID) {
    // loggerPtr->Logger::addLog(LogLevel::LOG_ERROR, "MODEL-setProgramID()", "doing nothing");
}

bool Model::eraseMaterial(unsigned int materialID) {
    bool found = false;

    for (MeshInstance& meshInstance : meshInstances) {
        if (meshInstance.materialID == materialID) {
            meshInstance.materialID = UINT_MAX;
            found = true;
        }
    }

    rebuildMaterialReferences();
    return found;
}

void Model::rebuildMaterialReferences() {
    allMaterialReferences.clear();

    for (const MeshInstance& meshInstance : meshInstances) {
        if (meshInstance.materialID != UINT_MAX) {
            if (allMaterialReferences.contains(meshInstance.materialID)) {
                allMaterialReferences.at(meshInstance.materialID)++;
            } else {
                allMaterialReferences.emplace(meshInstance.materialID, 1);
            }
        }
    }

    for (const MeshInstance& meshInstance : meshInstances) {
        if (meshInstance.materialID == UINT_MAX) {
            status.material = ModelState::Building;
            return;
        }
    }

    status.material = ModelState::Ready;
}