#include "Model.hpp"

#include "core/logging/LogSink.hpp"
#include "core/logging/Logger.hpp"
#include "engine/ShaderProgram.hpp"
#include "object/Material.hpp"
#include "texture/TextureCache.hpp"



Model::Model(const unsigned int ID, TextureCache* _textureCachePtr, Logger* _loggerPtr, MaterialCache* _materialCachePtr)
    : ID(ID), textureCachePtr(_textureCachePtr), loggerPtr(_loggerPtr), materialCachePtr(_materialCachePtr) {}


// -----FUNCTIONALITY
void Model::renderPrimitive(unsigned int meshID) {
    ModelPrimitive& prim = primitives[meshID];
    MeshA* mesh = all_meshes[prim.meshID].get();
    Material* mat = materialCachePtr->getMaterial(prim.materialID);

    if (mat == nullptr) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "Model::renderPrimitive", "Material not found!");
        return;
    }

    mesh->bind();

    std::vector<unsigned int> textureIDs = mat->getMaterialTextureIDs();
    if (textureIDs.empty()) {
        textureCachePtr->bindDefault();
    }
    else {
        unsigned int texUnit = 0;
        for (unsigned int texID : textureIDs) {
            textureCachePtr->bindTexture(texID, texUnit++);
        }
    }
    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}


void Model::unloadAllPrimitives() {
    for (ModelPrimitive& prim : primitives) {
        all_meshes[prim.meshID]->unloadFromGPU();
        // all_materials[prim.materialID]->unloadFromGPU();
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
    for (auto matID : all_material_ids) {
        Material* mat = materialCachePtr->getMaterial(matID);
        if (mat == nullptr) {
            loggerPtr->addLog(LogLevel::LOG_ERROR, "Model::setModelProgram", "material not found");
        }
        mat->setProgramID(programID);
    }
}


void Model::setMaterialProgram(unsigned int materialID, std::string& programID) {
    Material* mat = materialCachePtr->getMaterial(materialID);
    if (mat == nullptr) {
        loggerPtr->Logger::addLog(LogLevel::LOG_ERROR, "MODEL | setMaterialProgram()", "Material doesn't exist at index ", std::to_string(materialID));
        return;
    }
    mat->setProgramID(programID);
}


void Model::setMaterialType(unsigned int materialID, MaterialType type) {
    Material* mat = materialCachePtr->getMaterial(materialID);
    if (mat == nullptr) {
        loggerPtr->Logger::addLog(LogLevel::LOG_ERROR, "MODEL | setMaterialType()", "Material doesn't exist at index ", std::to_string(materialID));
        return;
    }
    mat->type = type;
}


void Model::calcModelM() {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model *= glm::mat4_cast(orientation); 
    model = glm::scale(model, scale);
    this->modelM = model;
}

const std::vector<unsigned int>& Model::getAllMaterialIDs() const {
    return all_material_ids;
}

MaterialType Model::getMaterialType(unsigned int materialID) const {
    Material* mat = materialCachePtr->getMaterial(materialID);
    if (mat == nullptr) {
        loggerPtr->Logger::addLog(LogLevel::LOG_ERROR, "MODEL | getMaterialType()", "Returning Cutout: Material doesn't exist at index ", std::to_string(materialID));
        return MaterialType::Cutout;
    }
    return mat->type;
}


std::string Model::getMaterialProgramID(unsigned int materialID) const {
    Material* mat = materialCachePtr->getMaterial(materialID);
    if (mat == nullptr) {
        loggerPtr->Logger::addLog(LogLevel::LOG_ERROR, "MODEL | getMaterialProgramID()", "Returning Cutout: Material doesn't exist at index ", std::to_string(materialID));
        return "";
    }
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
