#ifndef INSPECTOR_HPP
#define INSPECTOR_HPP

#include "../engine/ShaderProgram.hpp"
// #include "nlohmanjson.hpp"
#include <GLFW/glfw3.h>
#include <fstream>
#include <glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <iostream>
#include <math.h>
// #include <stb_image.h>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>

class Inspector {
  public:
    using UniformValue = std::variant<int, float, glm::vec3, glm::vec4>;
    enum class UniformType { NoType, Int, Float, Vec3, Vec4, UniformRef };
    std::unordered_map<std::string, UniformType> typeMap = {
        {"vec3", UniformType::Vec3},
        {"vec4", UniformType::Vec4},
        {"int", UniformType::Int},
        {"float", UniformType::Float}
    };
    struct WorldObject {
        std::string shaderUsed;
        std::string objectName;
        std::string objectPosition;
    };
    struct UniformRef {
        std::string shaderName;
        std::string uniformName;
        std::string uniformType;
    };
    struct Uniform {
        std::string name;
        std::string shaderName;
        UniformType type;
        UniformValue value;
        bool wasUniformRef = false;
        UniformRef ref;
    };

    // nlohmann::json json;
    std::string jsonPath;
    std::unordered_map<std::string, std::unordered_map<std::string, Uniform>>
        uniforms;
    std::vector<std::string> shaderPaths = {
        "../shaders/default.vert", "../shaders/default.frag",
    };
    std::unordered_map<std::string, ShaderProgram *> shaders;
    std::unordered_map<std::string, WorldObject> objects;

    std::string newUniformName;
    std::string newUniformShaderName;
    UniformType newUniformType = UniformType::NoType;
    std::string newObjectName;
    std::string newObjectShaderUsed;

    Inspector(GLFWwindow *window) {
        // note: You must initialize GFLW before doing thise!!!!
        std::cout
            << "Initializing Inspector... Make sure you terminate it later"
            << std::endl;
        shaders.insert_or_assign(
            "default",
            new ShaderProgram(shaderPaths[0].c_str(), shaderPaths[1].c_str(), "default"));
        uniforms["default"];
        /*
        jsonPath = "src/uniforms/project.json";

        std::fstream file(jsonPath, std::fstream::in | std::fstream::out |
                                        std::fstream::app);

        if (!file.is_open()) {
            std::cout << "failed to open or create uniforms file " << jsonPath
                      << std::endl;
            return;
        }

        if (file.peek() == std::ifstream::traits_type::eof()) {
            std::cout << "No uniforms initialized, making new json file"
                      << std::endl;
            // saveProjectJSON();
        }

        file.close();

        for (auto &[shaderName, shader] : shaders) {
            shader->use();
            // loadProjectJSON(shaderName);
        }
        */

    }

    /*
    bool initUniformValueFromJSON(Uniform *uniform, std::string shaderName) {
      // Set value
      ShaderProgram *shader = shaders[shaderName];
      switch (uniform->type) {
      case UniformType::NoType:
        std::cout << "Not a valid type! " << std::endl;
        return false; // failure
      case UniformType::Int: {
        int intValue =
            json["shaders"][shaderName]["uniforms"][uniform->name]["value"];
        shader->setUniform_int(uniform->name.c_str(), intValue);
        uniform->value = intValue;
        break;
      }
      case UniformType::Float: {
        std::cout << "initializing float " << uniform->name << std::endl;
        auto &val =
            json["shaders"][shaderName]["uniforms"][uniform->name]["value"];

        std::cout << "initializing " << val << std::endl;
        float floatValue = val.get<float>();
        shader->setUniform_float(uniform->name, floatValue);
        uniform->value = floatValue;
        break;
      }
      case UniformType::Vec3: {
        float vec3Value1, vec3Value2, vec3Value3;
        nlohmann::json valueJSON =
            json["shaders"][shaderName]["uniforms"][uniform->name]["value"];
        vec3Value1 = valueJSON[0];
        vec3Value2 = valueJSON[1];
        vec3Value3 = valueJSON[2];
        glm::vec3 vec3(vec3Value1, vec3Value2, vec3Value3);
        shader->setUniform_vec3float(uniform->name, vec3);
        uniform->value = vec3;
        break;
      }
      case UniformType::Vec4: {
        float vec4Value1, vec4Value2, vec4Value3, vec4Value4;
        nlohmann::json valueJSON =
            json["shaders"][shaderName]["uniforms"][uniform->name]["value"];
        vec4Value1 = valueJSON[0];
        vec4Value2 = valueJSON[1];
        vec4Value3 = valueJSON[2];
        vec4Value4 = valueJSON[3];
        glm::vec4 vec4(vec4Value1, vec4Value2, vec4Value3, vec4Value4);
        shader->setUniform_vec4float(uniform->name, vec4Value1, vec4Value2,
    vec4Value3, vec4Value4); uniform->value = vec4; break;
      }
      case UniformType::UniformRef: {
        // Later, I'd like to defer this so you don't need to have all your
        // uniform refs at the end of the file
        nlohmann::json valueJSON =
            json["shaders"][shaderName]["uniforms"][uniform->name]["value"];
        std::string refShaderName = valueJSON["shaderName"];
        std::string refName = valueJSON["uniformName"];

        // Uniform Refs need to be placed AFTER the uniform they're referencing.
        // If not, we need to warn them it's a bad file
        if (uniforms.count(refShaderName) == 0) {
          std::cout << "bad shader name!: " << refShaderName << std::endl;
          return false;
        } else if (uniforms[refShaderName].count(refName) == 0) {
          std::cout << "Uniform ref needs to be placed AFTER the uniform it's "
                       "referencing!"
                    << std::endl;
          return false;
        }

        Uniform ref = uniforms[refShaderName][refName];
        uniform->value = ref.value;
        uniform->type = ref.type;
        switch (ref.type) {
          uniform->wasUniformRef = true;
        case UniformType::Int:
          shader->setUniform_int(uniform->name.c_str(),
    std::get<int>(ref.value)); break; case UniformType::Float:
          shader->setUniform_float(uniform->name.c_str(),
    std::get<float>(ref.value)); break; case UniformType::Vec3:
          shader->setUniform_vec3float(uniform->name.c_str(),
    std::get<glm::vec3>(ref.value)); break; case UniformType::Vec4:
          shader->setUniform_vec4float(uniform->name.c_str(),
    std::get<glm::vec4>(ref.value)); break;
        }
        break;
      }
      }
      return true;
    }
    */

    bool initUniformValue(Uniform *uniform, std::string shaderName) {
        // Set value
        ShaderProgram *shader = shaders[shaderName];
        UniformValue value = uniform->value;
        switch (uniform->type) {
        case UniformType::NoType:
            std::cout << "Not a valid type! " << std::endl;
            return false; // failure
        case UniformType::Int: {
            shader->setUniform_int(uniform->name.c_str(), std::get<int>(value));
            break;
        }
        case UniformType::Float: {
            shader->setUniform_float(uniform->name.c_str(),
                                     std::get<float>(value));
            break;
        }
        case UniformType::Vec3: {
            shader->setUniform_vec3float(uniform->name.c_str(),
                                         std::get<glm::vec3>(value));
            break;
        }
        case UniformType::Vec4: {
            shader->setUniform_vec4float(uniform->name.c_str(),
                                         std::get<glm::vec4>(value));
            break;
        }
        case UniformType::UniformRef: {
            // Not supported yet
            break;
        }
        }
        return true;
    }

    // this assigns a uniform's value is it was set somewhere not in the inspector.
    void assignPreExistingValue(Uniform& uniform) {
        switch (uniform.type) {
        case UniformType::Int:
            uniform.value = shaders.at(uniform.shaderName.c_str())->getUniform_int(uniform.name.c_str());
            break;
        case UniformType::Float:
            uniform.value = shaders.at(uniform.shaderName.c_str())->getUniform_float(uniform.name.c_str());
            break;
        case UniformType::Vec3:
            uniform.value = shaders.at(uniform.shaderName.c_str())->getUniform_vec3float(uniform.name.c_str());
            break;
        case UniformType::Vec4:
            uniform.value = shaders.at(uniform.shaderName.c_str())->getUniform_vec4float(uniform.name.c_str());
            break;
        default:
            std::cout << "invalid new uniform type, making it an int"
                        << std::endl;
            uniform.type = UniformType::Int;
            uniform.value = 0;
            break;
        }
        std::cout << "assigned preexisting value: " << uniformValueToString(uniform) << " to " << uniform.name << std::endl;
    }

    void refreshUniforms(const std::string shaderName) {
        ShaderProgram *shader = shaders[shaderName];
        shader->use();

        std::unordered_map<std::string, Uniform> &shaderUniforms = uniforms.at(shaderName);
        std::unordered_map<std::string, Uniform> fileUniforms = readUniformsFromShader(shaderName);
        for (auto &[pairUniformName, pairUniform] : fileUniforms) {
            std::string uName = pairUniformName;
            Uniform u = pairUniform;
            bool isNew = shaderUniforms.count(uName) < 1;
            bool newType = false;
            if (!isNew) {
                newType = shaderUniforms.at(uName).type != u.type;
            }
            bool changeUniformMap = isNew || newType;

            if (changeUniformMap) {
                assignDefaultValue(u);
                shaderUniforms.insert_or_assign(uName, u);
            }
        }

        for (auto &[uniformName, uniform] : shaderUniforms) {
            initUniformValue(&uniform, shaderName);
        }
    }

    std::unordered_map<std::string, Uniform> readUniformsFromShader(std::string shaderName) {
        ShaderProgram &shader = *shaders[shaderName];
        std::stringstream vertCode(shader.vertShader_code);
        std::stringstream fragCode(shader.fragShader_code);
        std::unordered_map<std::string, Uniform> fileUniforms;

        std::string line;
        while (std::getline(vertCode, line)) {
            std::istringstream liness(line);
            std::string word;
            liness >> word;

            bool notUniformLine = word != "uniform";
            if (notUniformLine) continue;

            Uniform u;
            liness >> word;
            auto it = typeMap.find(word);
            if (it != typeMap.end()) {
                u.type = it->second;
            } else {
                std::cout << "Invalid Uniform Type! " << word << std::endl;
                continue;
            }
            u.wasUniformRef = false;
            liness >> u.name;
            if (!u.name.empty() && u.name.back() == ';') {
                u.name.pop_back();
            }
            u.shaderName = shaderName;
            fileUniforms[u.name] = u;
            std::cout << "read " << u.name << std::endl;
        }

        while (std::getline(fragCode, line)) {
            std::istringstream liness(line);
            std::string word;
            liness >> word;

            bool notUniformLine = word != "uniform";
            if (notUniformLine) continue;

            Uniform u;
            liness >> word;
            auto it = typeMap.find(word);
            if (it != typeMap.end()) {
                u.type = it->second;
            } else {
                std::cout << "Invalid Uniform Type! " << word << std::endl;
                continue;
            }
            u.wasUniformRef = false;
            liness >> u.name;
            if (!u.name.empty() && u.name.back() == ';') {
                u.name.pop_back();
            }
            u.shaderName = shaderName;
            fileUniforms[u.name] = u;
            std::cout << "read " << u.name << std::endl;
        }
        std::cout << "read " << fileUniforms.size() << " uniforms" << std::endl;
        return fileUniforms;
    }

    /*
    void loadProjectJSON(const std::string shaderName) {
      ShaderProgram *shader = shaders[shaderName];
      shader->use();

      std::fstream file(jsonPath,
                        std::fstream::in | std::fstream::out |
    std::fstream::app); if (!file.is_open()) { std::cout << "failed to open or
    create uniforms file " << jsonPath
                  << std::endl;
        return;
      }

      json = nlohmann::json::parse(file);
      std::cout << "json parsed, now loading uniforms" << std::endl;

      // see nlohmann json docs and lookup "structured bindings"
      for (auto &[uniformName, uniformJSON] :
           json["shaders"][shaderName]["uniforms"].items()) {
        Uniform uniform;
        uniform.shaderName = shaderName;
        uniform.name = uniformName;

        if (uniforms.count(shaderName) > 0 &&
            uniforms[shaderName].count(uniform.name) > 0) {
          std::cout << "duplicate name! " << uniform.name << std::endl;
        }

        uniform.type = uniformJSON["type"].get<UniformType>();

        std::cout << "loading " << uniform.name << std::endl;
        bool success = initUniformValueFromJSON(&uniform, shaderName);
        if (!success) {
          std::cout << "failed to read uniform: " << uniform.name << std::endl;
        } else {
          uniforms[uniform.shaderName][uniform.name] = uniform;
        }
      }
      file.close();
    }
    */

    /*
    void saveProjectJSON() {
      json["shaders"] = {};
      for (auto &[shaderName, shader] : shaders) {
        json["shaders"][shaderName] = {{"vertex", shader->vertexPath},
                                       {"fragment", shader->fragmentPath}};
        json["shaders"][shaderName]["uniforms"] = nlohmann::json::object();

        for (auto &[uniformName, uniform] : uniforms[shaderName]) {
          json["shaders"][shaderName]["uniforms"][uniformName] =
              nlohmann::json::object({{"type", uniform.type},
                                      {"wasUniformRef",
    uniform.wasUniformRef}}); switch (uniform.type) { case UniformType::Int: {
            int v = std::get<int>(uniform.value);
            json["shaders"][shaderName]["uniforms"][uniformName]["value"] = v;
            break;
          }
          case UniformType::Float: {
            float v = std::get<float>(uniform.value);
            json["shaders"][shaderName]["uniforms"][uniformName]["value"] = v;
            break;
          }
          case UniformType::Vec3: {
            glm::vec3 v = std::get<glm::vec3>(uniform.value);
            json["shaders"][shaderName]["uniforms"][uniformName]["value"] = {
                v.x, v.y, v.z};
            break;
          }
          case UniformType::Vec4: {
            glm::vec4 v = std::get<glm::vec4>(uniform.value);
            json["shaders"][shaderName]["uniforms"][uniformName]["value"] = {
                v.x, v.y, v.z, v.w};
            break;
          }
          case UniformType::UniformRef: {
            UniformRef ref = uniform.ref;
            json["shaders"][shaderName]["uniforms"][uniformName]["value"] = {
                {"shaderName", ref.shaderName},
                {"uniformName", ref.uniformName},
                {"type", ref.uniformType}};
            break;
          }
          }
        }
      }
      std::ofstream outfile(jsonPath);
      if (outfile.is_open()) {
        outfile << json.dump(4);
        outfile.close();
      }
    }
    */

    void refreshShaders() {
        std::cout << "Initializing Shaders..." << std::endl;
        shaders.clear();
        shaders.insert_or_assign(
            "default",
            new ShaderProgram(shaderPaths[0].c_str(), shaderPaths[1].c_str(), "default"));
        for (auto &[shaderName, shaderMap] : uniforms) {
            shaders.at(shaderName)->use();
            refreshUniforms(shaderName);
        }

        for (const auto &[shaderName, shaderUniforms] : uniforms) {
            std::cout << "Shader: " << shaderName << std::endl;

            for (const auto &[uniformName, uniform] : shaderUniforms) {
                std::cout << "  " << uniformName << " (type: ";
                if (uniform.wasUniformRef)
                    std::cout << " Uniform Ref -> ";
                std::cout << uniformValueToString(uniform);
            }
            std::cout << std::endl;
        }
    }

    std::string uniformValueToString(Uniform uniform) {
        std::stringstream ss;
        switch (uniform.type) {
        case UniformType::Int:
            ss << "Int) = " << std::get<int>(uniform.value);
            break;
        case UniformType::Float:
            ss << "Float) = " << std::get<float>(uniform.value);
            break;
        case UniformType::Vec3: {
            auto v = std::get<glm::vec3>(uniform.value);
            ss << "Vec3) = (" << v.x << ", " << v.y << ", "
                        << v.z << ")";
            break;
        }
        case UniformType::Vec4: {
            auto v = std::get<glm::vec4>(uniform.value);
            ss << "Vec4) = (" << v.x << ", " << v.y << ", "
                        << v.z << ", " << v.w << ")";
            break;
        }
        case UniformType::UniformRef:
            ss << "UniformRef)";
            break;
        default:
            ss << "Unknown)";
            break;
        }

        return ss.str();
    }

    void drawInspector() {
        ImGui::Text("Object Properties");

        drawUniformEditors();
    }

    void drawTextInput(std::string *value, const char *label) {
        char buffer[256];
        std::snprintf(buffer, sizeof(buffer), "%s", value->c_str());
        if (ImGui::InputText(label, buffer, sizeof(buffer))) {
            *value = buffer;
        }
    }

    void drawAddUniformMenu() {
        ImGui::Text("Add Uniform");
        drawTextInput(&newUniformName, "Uniform Name");

        // Build a list of shader names
        std::vector<const char *> shaderChoices;
        shaderChoices.reserve(shaders.size() + 1);
        shaderChoices.push_back("");
        for (auto &[name, shader] : shaders) {
            shaderChoices.push_back(name.c_str());
        }
        // Keep track of the selected shader index
        static int shaderChoice = 0;
        if (newUniformShaderName == "")
            shaderChoice = 0;
        // Display combo box
        if (ImGui::Combo("Shader", &shaderChoice, shaderChoices.data(),
                         (int)shaderChoices.size())) {
            // When selection changes, update newUniformShaderName
            newUniformShaderName = shaderChoices[shaderChoice];
        }
        // Dropdown menu from enum
        const char *typeNames[6] = {"None", "Int",  "Float",
                                    "Vec3", "Vec4", "Reference"};
        int choice = static_cast<int>(newUniformType);
        if (ImGui::Combo("Uniform Type", &choice, typeNames,
                         IM_ARRAYSIZE(typeNames))) {
            newUniformType = static_cast<UniformType>(choice);
        }

        if (ImGui::Button("Add Uniform", ImVec2(100, 20))) {
            bool uniqueValidUniform = false;
            bool validShaderName = shaders.count(newUniformShaderName) >= 1;
            if (validShaderName) {
                uniqueValidUniform =
                    uniforms[newUniformShaderName].count(newUniformName) <= 0 &&
                    newUniformType != UniformType::NoType;
            }

            if (uniqueValidUniform) {
                Uniform newUniform;
                newUniform.shaderName = newUniformShaderName;
                newUniform.name = newUniformName;
                newUniform.type = newUniformType;

                switch (newUniformType) {
                case UniformType::Int:
                    newUniform.value = 0;
                    break;
                case UniformType::Float:
                    newUniform.value = 0.0f;
                    break;
                case UniformType::Vec3:
                    newUniform.value = glm::vec3(0.0f);
                    break;
                case UniformType::Vec4:
                    newUniform.value = glm::vec4(0.0f);
                    break;
                default:
                    std::cout << "invalid new uniform type, making it an int"
                              << std::endl;
                    newUniform.type = UniformType::Int;
                    newUniform.value = 0;
                    break;
                }

                uniforms[newUniformShaderName][newUniformName] = newUniform;
                newUniformShaderName = "";
                newUniformName = "";
            } else {
                std::cout << "bad new uniform input" << std::endl;
            }
        }
    }

    void drawAddObjectMenu() {
        ImGui::Text("Add Object");
        drawTextInput(&newObjectName, "Object Name");

        // Build a list of shader names
        std::vector<const char *> shaderChoices;
        shaderChoices.reserve(shaders.size() + 1);
        shaderChoices.push_back("");
        for (auto &[name, shader] : shaders) {
            shaderChoices.push_back(name.c_str());
        }
        // Keep track of the selected shader index
        static int shaderChoice = 0;
        if (newObjectName == "")
            shaderChoice = 0;
        // Display combo box
        if (ImGui::Combo("Shader", &shaderChoice, shaderChoices.data(),
                         (int)shaderChoices.size())) {
            // When selection changes, update newObjectName
            newObjectName = shaderChoices[shaderChoice];
        }
        // Dropdown menu from enum
        const char *typeNames[6] = {"None", "Int",  "Float",
                                    "Vec3", "Vec4", "Reference"};
        int choice = static_cast<int>(newUniformType);
        if (ImGui::Combo("Uniform Type", &choice, typeNames,
                         IM_ARRAYSIZE(typeNames))) {
            newUniformType = static_cast<UniformType>(choice);
        }

        if (ImGui::Button("Add Uniform", ImVec2(100, 20))) {
            bool uniqueValidUniform = false;
            bool validShaderName = shaders.count(newObjectName) >= 1;
            if (validShaderName) {
                uniqueValidUniform =
                    uniforms[newObjectName].count(newUniformName) <= 0 &&
                    newUniformType != UniformType::NoType;
            }

            if (uniqueValidUniform) {
                Uniform newUniform;
                newUniform.shaderName = newObjectName;
                newUniform.name = newUniformName;
                newUniform.type = newUniformType;

                assignDefaultValue(newUniform);

                uniforms[newObjectName][newUniformName] = newUniform;
                newObjectName = "";
                newUniformName = "";
            } else {
                std::cout << "bad new uniform input" << std::endl;
            }
        }
    }

    void assignDefaultValue(Uniform& uniform) {
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

    std::vector<std::string> uniformNamesToDelete;

    void drawUniformEditors() {
        drawAddUniformMenu();
        int imGuiID = 0;
        for (auto &[shaderName, shaderMap] : uniforms) {
            ImGui::Text("%s", ("Shader:" + shaderName).c_str());
            for (auto &[uniformName, uniform] : uniforms.at(shaderName)) {
                ImGui::PushID(imGuiID);
                drawUniformInput(shaderName, uniformName);
                ImGui::PopID();
                imGuiID++;
            }
            for (std::string s : uniformNamesToDelete)
                uniforms.at(shaderName).erase(s);
            uniformNamesToDelete.clear();
        }
    }

    void drawUniformInput(std::string shaderName, std::string uniformName) {
        Uniform *uniform = &(uniforms[shaderName][uniformName]);
        ImGui::Text("%s", uniformName.c_str());
        switch (uniform->type) {
        case UniformType::Int: {
            int intValue = std::get<int>(uniform->value);
            ImGui::InputInt("value", &intValue);
            uniform->value = intValue;
            break;
        }
        case UniformType::Float: {
            float floatValue = std::get<float>(uniform->value);
            ImGui::InputFloat("value", &floatValue);
            uniform->value = floatValue;
            break;
        }
        case UniformType::Vec3: {
            glm::vec3 v3Val = std::get<glm::vec3>(uniform->value);
            ImGui::InputFloat("x", &(v3Val.x));
            ImGui::InputFloat("y", &(v3Val.y));
            ImGui::InputFloat("z", &(v3Val.z));
            uniform->value = v3Val;
            break;
        }
        case UniformType::Vec4: {
            glm::vec4 v4Val = std::get<glm::vec4>(uniform->value);
            ImGui::InputFloat("x", &(v4Val.x));
            ImGui::InputFloat("y", &(v4Val.y));
            ImGui::InputFloat("z", &(v4Val.z));
            ImGui::InputFloat("w", &(v4Val.w));
            uniform->value = v4Val;
            break;
        }
        case UniformType::UniformRef: {
            // Later, I'd like to defer this so you don't need to have all your
            // uniform refs at the end of the file
            std::cout << "no refs yet supported" << std::endl;
        }
        case UniformType::NoType: {
            std::cout << "Uniform has no type. How did you do that???" << std::endl;
        }
        
        }

        if (ImGui::Button("Delete Uniform", ImVec2(100, 20))) {
            uniformNamesToDelete.push_back(uniformName);
        }
    }
};
#endif
