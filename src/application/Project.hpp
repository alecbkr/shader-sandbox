#pragma once

#include "core/ShaderRegistry.hpp"
#include <string>
#include <filesystem>
#include <glm/glm.hpp>
#include "object/MeshProperties.hpp"
#include "object/Vertex.hpp"
#include "object/InstanceData.hpp"
#include "object/MaterialProperties.hpp"
#include "object/MaterialType.hpp"
#include "object/ModelType.hpp"

#include "../core/ConsoleTypes.hpp"
#include <vector>

#include "core/EventDispatcher.hpp"
#include "core/UniformRegistry.hpp"

class ShaderRegistry;
class ShaderProgram;


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
    bool previouslySaved = false;
    std::filesystem::path projectRoot;
    std::filesystem::path projectShadersDir;
    std::filesystem::path projectJSON;

    std::vector<ModelEntry> modelData;
    std::vector<MaterialEntry> materialData;

    std::unordered_map<std::string, std::unique_ptr<ShaderProgram>> programs;
    std::unordered_map<unsigned int, Uniform> uniforms;
    ShaderRegistry* shaderRegistry = nullptr;
    UniformRegistry* uniformRegistry = nullptr;
    EventDispatcher* events = nullptr;
    ConsoleToggles consoleSettings;
    std::vector<std::filesystem::path> openShaderFiles;
};
