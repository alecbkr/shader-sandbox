#include "InspectorEngine.hpp"

std::unordered_map<std::string, std::unordered_map<std::string, Uniform>> InspectorEngine::uniforms{};

const std::unordered_map<std::string, UniformType> InspectorEngine::typeMap = {
    {"vec3", UniformType::Vec3},
    {"vec4", UniformType::Vec4},
    {"int", UniformType::Int},
    {"float", UniformType::Float}
};

InspectorEngine::InspectorEngine() {
    std::cout << "Initializing Inspector Engine" << std::endl;
    auto& programs = ShaderHandler::getPrograms();
    for (auto& [programName, program] : programs) {
        auto uniformMap = parseUniforms(program);
        uniforms.insert({ programName, uniformMap });
        applyAllUniformsForProgram(programName);
    }
};

std::unordered_map<std::string, Uniform> InspectorEngine::parseUniforms(const ShaderProgram& program) {
    std::unordered_map<std::string, Uniform> programUniforms;

    std::string line;
    for (size_t i = 0; i < 2; i++) {
        std::stringstream sourceCode = i == 0 ?
            std::stringstream(program.vertShader_code) :
            std::stringstream(program.fragShader_code);
        
        while (std::getline(sourceCode, line)) {
            // Figure out if line is a uniform line
            std::istringstream line_ss(line);
            std::string word;
            line_ss >> word; // first word of the line (test for "uniform")

            bool isUniformLine = word == "uniform";
            if (!isUniformLine) continue; // not a uniform line

            // Determine the type of uniform
            Uniform uniform;
            line_ss >> word; // second word of the line (look for type of uniform)

            auto typePair = typeMap.find(word);
            if (typePair != typeMap.end()) {
                uniform.type = typePair->second;
            } else {
                std::cout << "Invalid Uniform Type!" << word << std::endl;
                continue;
            }
            assignDefaultValue(uniform);

            // Assign the uniform name
            line_ss >> uniform.name; // third word of the line (uniform's name)
            if (!uniform.name.empty() && uniform.name.back() == ';') {
                uniform.name.pop_back();
            }

            uniform.programName = program.name;
            programUniforms[uniform.name] = uniform;
            std::cout << "Read " << uniform.name << std::endl;
        }
    }

    // Find old program uniforms
    auto oldProgramPair = uniforms.find(program.name);
    if (oldProgramPair != uniforms.end()) {
        auto& oldProgramUniforms = oldProgramPair->second;
        
        // Loop through every new uniform
        for (auto& [uniformName, uniform] : programUniforms) {
            // Try to find a matching old uniform
            auto oldUniformPair = oldProgramUniforms.find(uniformName);
            if (oldUniformPair == oldProgramUniforms.end()) continue;
            const Uniform& oldUniform = oldUniformPair->second;

            // If old uniform type matches the new type, use old value
            if (uniform.type == oldUniform.type){
                uniform.value = oldUniform.value;
            }
        }
    }

    return programUniforms;
}

void InspectorEngine::assignDefaultValue(Uniform& uniform) {
    switch (uniform.type) {
    case UniformType::Int:
        uniform.value = 0;
        break;
    case UniformType::Float:
        uniform.value = 0.0f;
        break;
    case UniformType::Vec3:
        uniform.value = glm::vec3(0.0f);
        break;
    case UniformType::Vec4:
        uniform.value = glm::vec4(0.0f);
        break;
    default:
        std::cout << "invalid new uniform type, making it an int"
                    << std::endl;
        uniform.type = UniformType::Int;
        uniform.value = 0;
        break;
    }
}

void InspectorEngine::setUniform(const std::string& programName, const std::string& uniformName, UniformValue value) {
    auto programPair = uniforms.find(programName);
    if (programPair == uniforms.end()) {
        std::cout << "Set Uniform: No program with that name found in Inspector Engine uniforms" << std::endl;
        return;
    }

    auto& programUniforms = programPair->second;
    
    auto uniformPair = programUniforms.find(uniformName);
    if (uniformPair == programUniforms.end()) {
        std::cout << "Set Uniform: No uniform with that name found in Inspector Engine uniforms" << std::endl;
        return;
    }

    uniformPair->second.value = value;

    applyUniform(programName, uniformPair->second);
}

 void InspectorEngine::applyAllUniformsForProgram(const std::string& programName) {
    auto programPair = uniforms.find(programName);
    if (programPair == uniforms.end()) {
        std::cout << "Apply All Uniforms: No program with that name found in Inspector Engine uniforms" << std::endl;
        return;
    }

    ShaderProgram* program = ShaderHandler::getProgram(programName);
    
    auto& programUniforms = programPair->second;

    for (auto& [uniformName, uniform] : programUniforms) {
        applyUniform(*program, uniform);
    }
}

void InspectorEngine::applyUniform(const std::string& programName, Uniform& uniform) {
    ShaderProgram* program = ShaderHandler::getProgram(programName);
    applyUniform(*program, uniform);
}

void InspectorEngine::applyUniform(ShaderProgram& program, const Uniform& uniform) {
    program.use();
    switch (uniform.type)
    {
    case UniformType::Int:
        program.setUniform_int(uniform.name.c_str(), std::get<int>(uniform.value));
        break;

    case UniformType::Float:
        program.setUniform_float(uniform.name.c_str(), std::get<float>(uniform.value));
        break;

    case UniformType::Vec3:
        program.setUniform_vec3float(uniform.name.c_str(), std::get<glm::vec3>(uniform.value));
        break;

    case UniformType::Vec4:
        program.setUniform_vec4float(uniform.name.c_str(), std::get<glm::vec4>(uniform.value));
        break;
    
    default:
        std::cout << "Apply Uniform: Uniform with unsupported type found" << std::endl;
        break;
    }
}