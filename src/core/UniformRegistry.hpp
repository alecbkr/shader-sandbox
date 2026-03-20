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
    const Uniform* tryReadUniform(unsigned int matID, const std::string& uniformName) const; // return false if we didn't find it.
    const std::unique_ptr<std::unordered_map<std::string, Uniform>> tryReadUniforms(unsigned int matID) const;

    bool containsMaterial(unsigned int matID);
    bool containsUniform(unsigned int matID, const std::string& uniformName);
    void insertUniformMap(unsigned int matID, const std::unordered_map<std::string, Uniform>& map);
    void eraseUniform(unsigned int matID, const std::string& uniformName);
    void registerInspectorUniform(unsigned int matID, Uniform uniform);
    void registerSceneUniform(Uniform uniform);
    void registerModelUniform(unsigned int ModelID, Uniform uniform);
    void registerMaterialUniform(unsigned int MaterialID, Uniform uniform);
    const std::unique_ptr<std::unordered_map<std::string, Uniform>> tryReadSceneUniforms() const;
    const std::unique_ptr<std::unordered_map<std::string, Uniform>> tryReadModelUniforms(unsigned int modelID) const;
    const std::unique_ptr<std::unordered_map<std::string, Uniform>> tryReadMaterialUniforms(unsigned int materialID) const;

    private:
    unsigned int nextID = 0;
    bool initialized = false;
    Logger* loggerPtr = nullptr;
    Project* project = nullptr;
    std::unordered_map<unsigned int, std::unordered_map<std::string, unsigned int>> old_version_uniforms;

    // ALECS JUNK
    struct PairHash {
        std::size_t operator()(const std::pair<unsigned int,unsigned int>& p) const noexcept {
            return std::hash<unsigned int>()(p.first) ^ (std::hash<unsigned int>()(p.second) << 1);
        }
    };

    std::unordered_map<std::string, unsigned int> scene_uniforms;
    std::unordered_map<unsigned int, std::unordered_map<std::string, unsigned int>> model_uniforms;
    std::unordered_map<unsigned int, std::unordered_map<std::string, unsigned int>> material_uniforms;
};
