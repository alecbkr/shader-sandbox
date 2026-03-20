#include "ProjectLoader.hpp"
#include "engine/ShaderProgram.hpp"

#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include <application/Project.hpp>
#include <core/ShaderRegistry.hpp>
#include <persistence/UniformPersistence.hpp>

#include "core/EventTypes.hpp"

using json = nlohmann::json;

int ProjectLoader::version = 1;

// use this in the cpp file only, importers of the header file don't need to have this info.
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

            bool shaderExists = project.shaderRegistry->getProgram(shaderData.name) != nullptr;
            if (!shaderExists) {
                project.shaderRegistry->registerProgram(shaderData.vert_path, shaderData.frag_path, shaderData.name);
            }
            else {
                project.shaderRegistry->replaceProgram(shaderData.vert_path, shaderData.frag_path, shaderData.name);
                std::cerr << "Shader \"" << shaderData.name << "\" already exists, replacing!" << std::endl;
            }
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
        project.previouslySaved = j.value("previouslySaved", false);

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
                                OpenFilePayload{ filePath.string(), fileName, 0, false }
                            }
                        );
                    }
                }
            }
        }
    } catch (...) {
        std::cerr << "Error while loading projectJSON" << std::endl;
        return false;
    }


    return true;
}

void ProjectLoader::save(const Project& project) {
    std::filesystem::create_directories(project.projectRoot);
    std::filesystem::create_directories(project.projectShadersDir);

    json j;
    j["version"] = version;
    j["projectTitle"] = project.projectTitle;
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
    j["previouslySaved"] = project.previouslySaved;

    json openShaderFiles = json::array();
    for (const auto& filePath : project.openShaderFiles) {
        openShaderFiles.push_back(filePath);
    }
    j["openShaderFiles"] = openShaderFiles;

    std::ofstream out(project.projectJSON);
    out << j.dump(4);

}
