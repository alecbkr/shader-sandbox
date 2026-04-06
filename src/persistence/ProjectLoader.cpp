#include "ProjectLoader.hpp"
#include "engine/ShaderProgram.hpp"

#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include <application/Project.hpp>
#include "object/AssimpImporter.hpp"
#include "object/ModelCache.hpp"
#include "object/MaterialCache.hpp"
#include <core/ShaderRegistry.hpp>
#include <persistence/UniformPersistence.hpp>

#include "core/EventTypes.hpp"

using json = nlohmann::json;

int ProjectLoader::version = 1;

namespace {
    struct {
        const char* listLabel = "programs";
        const char* name      = "name";
        const char* vertPath  = "vert_path";
        const char* fragPath  = "frag_path";
        const char* compiled  = "isCompiled";
    } shaderLabels;

    struct ShaderData {
        bool isCompiled;
        std::string name;
        std::string vert_path;
        std::string frag_path;
    };
}

inline void to_json(json& j, const InstanceData& m) {
    j = {m.pos.x, m.pos.y, m.pos.z};
}

inline void to_json(json& j, const ModelEntry& modelData){
    j = {
        {"name", modelData.name},
        {"ID", modelData.ID},
        {"path", modelData.path.string()},
        {"type", static_cast<int>(modelData.type)},

        {"meshMaterialIDs", modelData.meshMaterialIDs},
        {"instanceData", modelData.instanceData},
        {"position", {modelData.position.x, modelData.position.y, modelData.position.z}},
        {"scale", {modelData.scale.x, modelData.scale.y, modelData.scale.z}},
        {"rotation", {modelData.rotation.x, modelData.rotation.y, modelData.rotation.z, modelData.rotation.w}}
    };
}

inline void to_json(json& j, const MaterialProperties& matProps) {
    j = {
        {"metalness", matProps.metalness},
        {"opacity", matProps.opacity},
        {"roughness", matProps.roughness},
        {"shininess", matProps.shininess}
    };
}

inline void to_json(json& j, const MaterialEntry& materialData) {
    j = {
        {"name", materialData.name},
        {"ID", materialData.ID},
        {"type", static_cast<int>(materialData.type)},

        {"properties", materialData.properties},
        {"texture_paths", materialData.texture_paths},
        {"programID", materialData.programID}
    };
}


inline void from_json(const json& j, InstanceData& instData) {
    instData.pos.x = j.at(0).get<float>();
    instData.pos.y = j.at(1).get<float>();
    instData.pos.z = j.at(2).get<float>();
}

inline void from_json(const json& j, ModelEntry& modelData) {
    modelData.name = j.at("name").get<std::string>();
    modelData.ID = j.at("ID").get<unsigned int>();
    modelData.path = j.at("path").get<std::string>();
    modelData.type = static_cast<ModelType>(j.at("type").get<int>());

    modelData.meshMaterialIDs = j.at("meshMaterialIDs").get<std::vector<unsigned int>>();
    modelData.instanceData = j.at("instanceData").get<std::vector<InstanceData>>(); // uses InstanceData::from_json

    auto pos = j.at("position");
    modelData.position = glm::vec3(
        pos.at(0).get<float>(),
        pos.at(1).get<float>(),
        pos.at(2).get<float>()
    );

    auto scale = j.at("scale");
    modelData.scale = glm::vec3(
        scale.at(0).get<float>(),
        scale.at(1).get<float>(),
        scale.at(2).get<float>()
    );

    auto rotation = j.at("rotation");
    modelData.rotation = glm::vec4(
        rotation.at(0).get<float>(),
        rotation.at(1).get<float>(),
        rotation.at(2).get<float>(),
        rotation.at(3).get<float>()
    );
}

inline void from_json(const json& j, MaterialProperties& matProps) {
    matProps.metalness = j.at("metalness").get<float>();
    matProps.opacity = j.at("opacity").get<float>();
    matProps.roughness = j.at("roughness").get<float>();
    matProps.shininess = j.at("shininess").get<float>();
}

inline void from_json(const json& j, MaterialEntry& materialData) {
    materialData.name = j.at("name").get<std::string>();
    materialData.ID = j.at("ID").get<unsigned int>();
    materialData.type = static_cast<MaterialType>(j.at("type").get<int>());

    materialData.properties = j.at("properties").get<MaterialProperties>();
    materialData.texture_paths = j.at("texture_paths").get<std::vector<std::string>>();
    materialData.programID = j.at("programID").get<unsigned int>();
}


// returns success
bool loadShaders(Project& project, json& j) {
    if (project.shaderRegistry == nullptr) {
        std::cerr << "ShaderRegistry is null; cannot load shaders." << std::endl;
        return false;
    }
    try {
        json shaderList = j.value(shaderLabels.listLabel, json::array());
        if (shaderList.empty()) {
            std::cerr << "shaderList was empty!" << std::endl;
            return false;
        }

        for (const json& d : shaderList) {
            ShaderData shaderData;

            // Check that all required fields exist
            if (!d.contains(shaderLabels.name) ||
                !d.contains(shaderLabels.vertPath) ||
                !d.contains(shaderLabels.fragPath) ||
                !d.contains(shaderLabels.compiled))
            {
                std::cerr << "Shader entry missing required field(s)!" << std::endl;
                return false;
            }

            // Load fields into shaderData
            shaderData.name       = d.at(shaderLabels.name).get<std::string>();
            shaderData.vert_path  = d.at(shaderLabels.vertPath).get<std::string>();
            shaderData.frag_path  = d.at(shaderLabels.fragPath).get<std::string>();
            shaderData.isCompiled = d.at(shaderLabels.compiled).get<bool>();

            project.shaderRegistry->registerProgram(shaderData.vert_path, shaderData.frag_path, shaderData.name);
            // ShaderProgram* prevProg = project.shaderRegistry->getProgram(shaderData.ID);
            // bool shaderExists = prevProg != nullptr;
            // if (!shaderExists) {
            //     project.shaderRegistry->registerProgram(shaderData.vert_path, shaderData.frag_path, shaderData.name, shaderData.ID);
            // }
            // else {
            //     project.shaderRegistry->replaceProgram(shaderData.vert_path, shaderData.frag_path, shaderData.name);
            //     std::cerr << "Shader \"" << shaderData.name << "\" already exists, replacing!" << std::endl;
            // }
        }
    }
    catch (...) {
        std::cerr << "Error while loading projectJSON (Shaders)" << std::endl;
        return false;
    }

    return true;
}

// returns success
bool saveShaders(const Project& project, json& j) {
    j[shaderLabels.listLabel] = json::array();

    for (const auto& [name, prog] : project.programs) {
        j[shaderLabels.listLabel].push_back({
            {shaderLabels.name,     prog->name},
            {shaderLabels.vertPath, prog->vertPath},
            {shaderLabels.fragPath, prog->fragPath},
            {shaderLabels.compiled, prog->isCompiled()}
        });
    }
    return true;
}

bool ProjectLoader::loadAssets(Project& project) {
    if (!std::filesystem::exists(project.projectJSON)) 
    {
        std::cerr << "projectJSON does not exist" << std::endl;
        return false;
    }

    std::ifstream in(project.projectJSON);
    if (!in.is_open()) {
        std::cerr << "Could not open projectJSON" << std::endl;
        return false;
    }

    try {
        json j;
        in >> j;

        project.modelData = j.value("modelData", std::vector<ModelEntry>{});
        project.materialData = j.value("materialData", std::vector<MaterialEntry>{});

    } catch (const nlohmann::json::exception& e) {
        std::cerr << "JSON error: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Standard exception: " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "Unknown exception while loading projectJSON" << std::endl;
        return false;
    }
    return true;
}

bool ProjectLoader::load(Project& project) {
    if (!std::filesystem::exists(project.projectJSON)) 
    {
        std::cerr << "projectJSON does not exist" << std::endl;
        return false;
    }

    std::ifstream in(project.projectJSON);
    if (!in.is_open()) {
        std::cerr << "Could not open projectJSON" << std::endl;
        return false;
    }

    try {
        json j;
        in >> j;

        ProjectLoader::version = j.value("version", 1);
        project.projectTitle = j.value("projectTitle", project.projectTitle);

        if (!loadShaders(project, j)) {
            return false;
        }
        UniformPersistence uniformLoader;
        if (!uniformLoader.load(project, j)) {
            std::cerr << "Error while saving projectJSON (Uniforms)" << std::endl;
            return false;
        }
        project.consoleSettings = j.value("consoleSettings", project.consoleSettings);
        // project.previouslySaved = j.value("previouslySaved", false);

        if (project.events != nullptr) {
            if (j.contains("openShaderFiles") && j["openShaderFiles"].is_array()) {
                const auto& saved = j["openShaderFiles"];

                for (auto& fileName : saved) {
                    if (fileName.is_string()) {
                        std::filesystem::path filePath = project.projectShadersDir / fileName;
                        project.openShaderFiles.push_back(filePath);
                        project.events->TriggerEvent(
                            Event{
                                EventType::OpenFile,
                                false,
                                OpenFilePayload{ filePath.string(), fileName, false }
                            }
                        );
                    }
                }
            }
            project.previouslySaved = j.value("previouslySaved", true);
        }
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "JSON error: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Standard exception: " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "Unknown exception while loading projectJSON" << std::endl;
        return false;
    }


    return true;
}

void ProjectLoader::save(Project& project, ModelCache* modelCachePtr, MaterialCache* materialCachePtr) {
    std::filesystem::create_directories(project.projectRoot);
    std::filesystem::create_directories(project.projectShadersDir);

    project.modelData.clear();
    for (auto& model : modelCachePtr->getAllModels()) {
        ModelEntry modelEntry = {
            .name = model->getName(),
            .ID = model->getID(),
            .path = model->getPath(),
            .type = model->type,

            .meshMaterialIDs = model->getAllMaterialIDsPerMesh(),
            .instanceData = model->getInstanceData(),
            .position = model->getPosition(),
            .scale = model->getScale(),
            .rotation = model->getRotation()
        };
        project.modelData.push_back(modelEntry);
    }

    project.materialData.clear();
    for (auto& material : materialCachePtr->getAllMaterials()) {
        MaterialEntry materialEntry{
            .name = material->getName(),
            .ID = material->ID,
            .type = material->getMaterialType(),
            
            
            .properties = material->properties,
            .texture_paths = materialCachePtr->getAllTexturePathsForMaterial(material->ID),
            .programID = material->getProgramID()
        };
        project.materialData.push_back(materialEntry);
    }
    

    json j;
    j["version"] = version;
    j["projectTitle"] = project.projectTitle;
    j["modelData"] = project.modelData;
    j["materialData"] = project.materialData;
    
    j["consoleSettings"] = project.consoleSettings;

    if (!saveShaders(project, j)) {
        std::cerr << "Error while saving projectJSON (Shaders)" << std::endl;
        return;
    }
    UniformPersistence uniformSaver;
    if (!uniformSaver.save(project, j)) {
        std::cerr << "Error while saving projectJSON (Uniforms)" << std::endl;
        return;
    }
    // j["previouslySaved"] = project.previouslySaved;
    j["previouslySaved"] = true;

    json openShaderFiles = json::array();
    for (const auto& filePath : project.openShaderFiles) {
        openShaderFiles.push_back(filePath);
    }
    j["openShaderFiles"] = openShaderFiles;

    std::ofstream out(project.projectJSON);
    out << j.dump(4);
}
