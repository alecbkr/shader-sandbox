#include "Model.hpp"

#include "core/logging/Logger.hpp"
#include "engine/ShaderProgram.hpp"
#include "../texture/CubeMap.hpp"
#include "../texture/Texture2D.hpp"



Model::Model(const unsigned int ID, ShaderRegistry* _shaderRegPtr, Logger* _loggerPtr)
    : ID(ID), shaderRegPtr(_shaderRegPtr), loggerPtr(_loggerPtr) {}


// -----FUNCTIONALITY
void Model::renderPrimitive(unsigned int meshID) {
    ModelPrimitive& prim = primitives[meshID];
    MeshA* mesh = all_meshes[prim.meshID].get();
    Material* mat = all_materials[prim.materialID].get();

    mesh->bind();
    mat->bindTextures();
    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}


void Model::unloadAllPrimitives() {
    for (ModelPrimitive& prim : primitives) {
        all_meshes[prim.meshID]->unloadFromGPU();
        // all_materials[prim.materialID]->
    }
}


// -----TRANSLATIONS
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


void Model::translate(glm::vec3 vector) {
    position += vector;
    calcModelM();
}


void Model::rescale(glm::vec3 vector) {
    scale *= vector;
    calcModelM();
}


void Model::rotate(float angle, glm::vec3 axis) {
    float processedAngle = fmod((rotation.x + angle), 360.0f);
    if (processedAngle < 0) processedAngle += 360.0f;

    glm::vec3 processedAxis = glm::clamp(glm::vec3(rotation.y+axis.x, rotation.z+axis.y, rotation.w+axis.z), -1.0f, 1.0f);
    glm::vec3 normalizedAxis = processedAxis;
    if (glm::length(normalizedAxis) > 0.0001f) {
        normalizedAxis = glm::normalize(normalizedAxis);
    }

    rotation = glm::vec4(processedAngle, processedAxis);
    this->orientation = glm::angleAxis(glm::radians(processedAngle), normalizedAxis);
    calcModelM();
}


// -----SETTERS
void Model::setMesh(std::vector<float> vertices, std::vector<unsigned int> indices, bool hasPos, bool hasNorm, bool hasUV) {

}


void Model::setModelProgram(std::string& programID) {
    for (auto& mat : all_materials) {
        mat->setProgramID(programID);
    }
}


void Model::setMaterialProgram(unsigned int materialID, std::string& programID) {
    if (all_materials.size() - 1 < materialID) {
        loggerPtr->Logger::addLog(LogLevel::LOG_ERROR, "MODEL | setMaterialProgram()", "Material doesn't exist at index ", std::to_string(materialID));
        return;
    }
    all_materials[materialID]->setProgramID(programID);
}


void Model::setMaterialType(unsigned int materialID, MaterialType type) {
    if (all_materials.size() - 1 < materialID) {
        loggerPtr->Logger::addLog(LogLevel::LOG_ERROR, "MODEL | setMaterialType()", "Material doesn't exist at index ", std::to_string(materialID));
        return;
    }
    all_materials[materialID]->type = type;
}


void Model::calcModelM() {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model *= glm::mat4_cast(orientation); 
    model = glm::scale(model, scale);
    this->modelM = model;
}


MaterialType Model::getMaterialType(unsigned int materialID) const {
    Material* mat = all_materials[materialID].get();
    return mat->type;
}


std::string Model::getMaterialProgramID(unsigned int materialID) const {
    Material *mat = all_materials[materialID].get();
    return mat->getProgramID();
}


glm::mat4 Model::getModelMatrix() const {return modelM;}
glm::vec3 Model::getPosition()    const {return position;}
glm::vec3 Model::getScale()       const {return scale;}
glm::vec4 Model::getRotation()    const {return rotation;}


std::string Model::getProgramID() {
    // Logger::addLog(LogLevel::ERROR, "MODEL-getProgramID()", "outdated getProgramID call, returning dummy value");
    return "";
}


void Model::setProgramID(std::string programID) {
    // loggerPtr->Logger::addLog(LogLevel::LOG_ERROR, "MODEL-setProgramID()", "doing nothing");
}


// std::shared_ptr<Texture> Model::loadTexture(std::string texture_path, TextureType type) {

//     // if (type == TEX_CUBEMAP) {
//         // all_textures.try_emplace(
//         //     texture_path, 
//         //     std::make_shared<CubeMap>(texture_path)
//         // );

        
//     // }
//     // else {
//     //     all_textures.try_emplace(
//     //         texture_path, 
//     //         std::make_shared<Texture2D>(texture_path, type)
//     //     );
//     // }
//     std::shared_ptr<Texture> newTexture;
//     switch (type) {
//         case TextureType::TEX_CUBEMAP: newTexture = std::make_shared<CubeMap>(texture_path); break;
//         default:                       newTexture = std::make_shared<Texture2D>(texture_path, type);
//     }

//     return newTexture;
// }