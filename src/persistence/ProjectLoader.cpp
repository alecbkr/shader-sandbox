#include "ProjectLoader.hpp"

#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include "application/AppContext.hpp"
#include "object/AssimpImporter.hpp"
#include "object/ModelCache.hpp"
#include "object/MaterialCache.hpp"



using json = nlohmann::json;

int ProjectLoader::version = 1;

inline void to_json(json& j, const InstanceData& m) {
    j = {m.x_offset, m.y_offset, m.z_offset};
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
    instData.x_offset = j.at(0).get<float>();
    instData.y_offset = j.at(1).get<float>();
    instData.z_offset = j.at(2).get<float>();
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
    materialData.programID = j.at("programID").get<std::string>();
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

void ProjectLoader::save(Project& project, ModelCache* modelCachePtr, MaterialCache* materialCachePtr) {
    std::filesystem::create_directories(project.projectRoot);
    std::filesystem::create_directories(project.projectShadersDir);

    project.modelData.clear();
    for (auto& model : modelCachePtr->getAllModels()) {
        ModelEntry modelEntry = {
            .name = model->name,
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
            .name = material->name,
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

    std::ofstream out(project.projectJSON);
    out << j.dump(4);
}
