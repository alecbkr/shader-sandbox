#pragma once 
#include "UniformTypes.hpp"
#include <unordered_map>
#include <string>

class Logger;

class UniformRegistry {
    public:
    UniformRegistry();
    bool initialize(Logger* _loggerPtr);
    void shutdown();
    const Uniform* tryReadUniform(unsigned int modelID, const std::string& uniformName) const; // return false if we didn't find it.
    const std::unordered_map<std::string, Uniform>* tryReadUniforms(unsigned int modelID) const;
    bool containsObject(unsigned int modelID);
    bool containsUniform(unsigned int modelID, const std::string& uniformName);
    void registerUniform(unsigned int modelID, Uniform uniform);
    void insertUniformMap(unsigned int modelID, const std::unordered_map<std::string, Uniform>& map);
    void eraseUniform(unsigned int modelID, const std::string& uniformName);


    void registerSceneUniform(Uniform uniform);
    void registerModelUniform(unsigned int ModelID, Uniform uniform);
    void registerMaterialUniform(unsigned int ModelID, unsigned int MaterialID, Uniform uniform);
    const std::unordered_map<std::string, Uniform>* tryReadSceneUniforms() const;
    const std::unordered_map<std::string, Uniform>* tryReadModelUniforms(unsigned int modelID) const;
    const std::unordered_map<std::string, Uniform>* tryReadMaterialUniforms(unsigned int modelID, unsigned int materialID) const;

    private:
    bool initialized = false;
    Logger* loggerPtr = nullptr;
    std::unordered_map<unsigned int, std::unordered_map<std::string, Uniform>> uniforms;

    // ALECS JUNK
    struct PairHash {
        std::size_t operator()(const std::pair<unsigned int,unsigned int>& p) const noexcept {
            return std::hash<unsigned int>()(p.first) ^ (std::hash<unsigned int>()(p.second) << 1);
        }
    };

    std::unordered_map<std::string, Uniform> scene_uniforms;
    std::unordered_map<unsigned int, std::unordered_map<std::string, Uniform>> model_uniforms;
    std::unordered_map<std::pair<unsigned int, unsigned int>, std::unordered_map<std::string, Uniform>, PairHash> material_uniforms;
};
