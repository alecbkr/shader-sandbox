#pragma once 
#include "UniformTypes.hpp"
#include <unordered_map>
#include <string>
#include <memory>

class Logger;
struct Project;

class UniformRegistry {
    public:
    UniformRegistry();
    bool initialize(Logger* _loggerPtr, Project* _project);
    void shutdown();
    const std::unordered_map<unsigned int, Uniform>& readUniforms();

    void registerSceneUniform(Uniform uniform);
    void registerModelUniform(unsigned int modelID, Uniform uniform);
    void registerMaterialUniform(unsigned int matID, Uniform uniform);
    void registerMaterialUniformMap(unsigned int matID, std::unordered_map<std::string, Uniform>& map);

    const std::unique_ptr<std::unordered_map<std::string, Uniform>> tryReadSceneUniforms() const;
    const std::unique_ptr<std::unordered_map<std::string, Uniform>> tryReadModelUniforms(unsigned int modelID) const;
    const std::unique_ptr<std::unordered_map<std::string, Uniform>> tryReadMaterialUniforms(unsigned int materialID) const;

    size_t getSceneUniformsSize();
    size_t getModelUniformsSize(unsigned int modelID);
    size_t getMaterialUniformsSize(unsigned int matID);

    bool updateUniform(unsigned int id, Uniform uniform);
    const Uniform* tryReadUniform(unsigned int);

    // Functions for material uniforms. These are the ones users will be editing & defining.
    const Uniform* tryReadMaterialUniform(unsigned int matID, const std::string& uniformName) const; // return false if we didn't find it.
    bool containsMaterial(unsigned int matID);
    bool containsMaterialUniform(unsigned int matID, const std::string& uniformName);
    void eraseMaterialUniform(unsigned int matID, const std::string& uniformName);
    void eraseMaterial(unsigned int matID);

    private:
    unsigned int nextID = 0;
    bool initialized = false;
    Logger* loggerPtr = nullptr;
    Project* project = nullptr;
    std::unordered_map<std::string, unsigned int> sceneUniforms; // Uniforms global to the scene
    std::unordered_map<unsigned int, std::unordered_map<std::string, unsigned int>> modelUniforms; // uniforms that apply to every material in the model
    std::unordered_map<unsigned int, std::unordered_map<std::string, unsigned int>> materialUniforms;
};
