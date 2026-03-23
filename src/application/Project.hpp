#pragma once

#include <string>
#include <filesystem>
#include <glm/glm.hpp>
#include "object/MeshProperties.hpp"
#include "object/Vertex.hpp"
#include "object/InstanceData.hpp"
#include "object/MaterialProperties.hpp"
#include "object/MaterialType.hpp"
#include "object/ModelType.hpp"


struct ModelEntry {
    std::string name;
    unsigned int ID;
    std::filesystem::path path;
    ModelType type; //enum

    std::vector<unsigned int> meshMaterialIDs;
    std::vector<InstanceData> instanceData; //struct
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec4 rotation;
};


struct MaterialEntry {
    std::string name;
    unsigned int ID;
    MaterialType type; //enum

    MaterialProperties properties; //struct
    std::vector<std::string> texture_paths;
    std::string programID;
};

struct Project {
    std::string projectTitle;
    std::filesystem::path projectRoot;
    std::filesystem::path projectShadersDir;
    std::filesystem::path projectJSON;

    std::vector<ModelEntry> modelData;
    std::vector<MaterialEntry> materialData;
};