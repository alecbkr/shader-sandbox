#include "UniformPersistence.hpp"

#include "application/Project.hpp"
#include "core/UniformRegistry.hpp"
#include "core/UniformTypes.hpp"

#include <iostream>

using json = nlohmann::json;

namespace {
    struct {
        const char* listLabel           = "uniforms";
        const char* materialId          = "material_id";
        const char* name                = "name";
        const char* type                = "type";
        const char* value               = "value";
        const char* isFunction          = "isFunction";
        const char* isReadOnly          = "isReadOnly";
        const char* useAlternateEditor  = "useAlternateEditor";

        const char* kind                = "kind";
        const char* v                   = "v";
        const char* textureUnit         = "texture_unit";

        const char* valueKindInt        = "int";
        const char* valueKindFloat      = "float";
        const char* valueKindVec3       = "vec3";
        const char* valueKindVec4       = "vec4";
        const char* valueKindMat4       = "mat4";
        const char* valueKindSampler2d  = "sampler2d";
        const char* valueKindReference  = "reference";
    } uniformLabels;

    struct {
        const char* modelSelection       = "modelSelection";
        const char* uniformSelection     = "uniformSelection";
        const char* materialSelection    = "materialSelection";
        const char* referencedModelID    = "referencedModelID";
        const char* referencedMaterialID = "referencedMaterialID";
        const char* referencedUniformName = "referencedUniformName";
        const char* returnType           = "returnType";
        const char* useWorldData         = "useWorldData";
        const char* useCamaraData        = "useCamaraData";
        const char* initialized          = "initialized";
    } referenceLabels;

    template <class... Ts>
    struct overloaded : Ts... {
        using Ts::operator()...;
    };
    template <class... Ts>
    overloaded(Ts...) -> overloaded<Ts...>;

    std::optional<UniformType> parseUniformType(const std::string& s) {
        for (int i = static_cast<int>(UniformType::NoType); i <= static_cast<int>(UniformType::SamplerCube); ++i) {
            auto t = static_cast<UniformType>(i);
            if (to_string(t) == s) {
                return t;
            }
        }
        return std::nullopt;
    }

    json uniformValueToJson(const UniformValue& v) {
        return std::visit(
            overloaded{
                [](int x) {
                    return json{ { uniformLabels.kind, uniformLabels.valueKindInt }, { uniformLabels.v, x } };
                },
                [](float x) {
                    return json{ { uniformLabels.kind, uniformLabels.valueKindFloat }, { uniformLabels.v, x } };
                },
                [](const glm::vec3& x) {
                    return json{
                        { uniformLabels.kind, uniformLabels.valueKindVec3 },
                        { uniformLabels.v, json::array({ x.x, x.y, x.z }) }
                    };
                },
                [](const glm::vec4& x) {
                    return json{
                        { uniformLabels.kind, uniformLabels.valueKindVec4 },
                        { uniformLabels.v, json::array({ x.x, x.y, x.z, x.w }) }
                    };
                },
                [](const glm::mat4& m) {
                    json arr = json::array();
                    for (int c = 0; c < 4; ++c) {
                        for (int r = 0; r < 4; ++r) {
                            arr.push_back(m[c][r]);
                        }
                    }
                    return json{ { uniformLabels.kind, uniformLabels.valueKindMat4 }, { uniformLabels.v, arr } };
                },
                [](const InspectorSampler2D& s) {
                    return json{
                        { uniformLabels.kind, uniformLabels.valueKindSampler2d },
                        { uniformLabels.textureUnit, s.textureUnit }
                    };
                },
                [](const InspectorReference& ref) {
                    return json{
                        { uniformLabels.kind, uniformLabels.valueKindReference },
                        { referenceLabels.modelSelection, ref.modelSelection },
                        { referenceLabels.uniformSelection, ref.uniformSelection },
                        { referenceLabels.materialSelection, ref.materialSelection },
                        { referenceLabels.referencedModelID, ref.referencedModelID },
                        { referenceLabels.referencedMaterialID, ref.referencedMaterialID },
                        { referenceLabels.referencedUniformName, ref.referencedUniformName },
                        { referenceLabels.returnType, to_string(ref.returnType) },
                        { referenceLabels.useWorldData, ref.useWorldData },
                        { referenceLabels.useCamaraData, ref.useCamaraData },
                        { referenceLabels.initialized, ref.initialized },
                    };
                },
            },
            v);
    }

    bool jsonToUniformValue(const json& j, UniformValue& out) {
        if (!j.is_object() || !j.contains(uniformLabels.kind)) {
            return false;
        }
        const std::string kind = j.at(uniformLabels.kind).get<std::string>();
        try {
            if (kind == uniformLabels.valueKindInt) {
                out = j.at(uniformLabels.v).get<int>();
                return true;
            }
            if (kind == uniformLabels.valueKindFloat) {
                out = j.at(uniformLabels.v).get<float>();
                return true;
            }
            if (kind == uniformLabels.valueKindVec3) {
                const auto& a = j.at(uniformLabels.v);
                out = glm::vec3(a.at(0).get<float>(), a.at(1).get<float>(), a.at(2).get<float>());
                return true;
            }
            if (kind == uniformLabels.valueKindVec4) {
                const auto& a = j.at(uniformLabels.v);
                out = glm::vec4(
                    a.at(0).get<float>(), a.at(1).get<float>(), a.at(2).get<float>(), a.at(3).get<float>());
                return true;
            }
            if (kind == uniformLabels.valueKindMat4) {
                const auto& a = j.at(uniformLabels.v);
                if (!a.is_array() || a.size() != 16) {
                    return false;
                }
                glm::mat4 m(1.0f);
                int i = 0;
                for (int c = 0; c < 4; ++c) {
                    for (int r = 0; r < 4; ++r) {
                        m[c][r] = a.at(i++).get<float>();
                    }
                }
                out = m;
                return true;
            }
            if (kind == uniformLabels.valueKindSampler2d) {
                out = InspectorSampler2D{ .textureUnit = j.at(uniformLabels.textureUnit).get<int>() };
                return true;
            }
            if (kind == uniformLabels.valueKindReference) {
                InspectorReference ref;
                ref.modelSelection       = j.value(referenceLabels.modelSelection, 0);
                ref.uniformSelection     = j.value(referenceLabels.uniformSelection, 0);
                ref.materialSelection    = j.value(referenceLabels.materialSelection, 0);
                ref.referencedModelID    = j.value(referenceLabels.referencedModelID, 0u);
                ref.referencedMaterialID = j.value(referenceLabels.referencedMaterialID, 0u);
                ref.referencedUniformName =
                    j.value(referenceLabels.referencedUniformName, std::string{});
                if (auto rt = parseUniformType(
                        j.value(referenceLabels.returnType, std::string(to_string(UniformType::NoType))))) {
                    ref.returnType = *rt;
                }
                else {
                    ref.returnType = UniformType::NoType;
                }
                ref.useWorldData  = j.value(referenceLabels.useWorldData, false);
                ref.useCamaraData = j.value(referenceLabels.useCamaraData, false);
                ref.initialized   = j.value(referenceLabels.initialized, false);
                out               = ref;
                return true;
            }
        }
        catch (...) {
            return false;
        }
        return false;
    }
}

bool UniformPersistence::load(Project& project, json& j) {
    UniformRegistry* uniReg = project.uniformRegistry;
    if (uniReg == nullptr) {
        std::cerr << "Could not load uniforms, uniform registry is null" << std::endl;
        return false;
    }

    try {
        json uniformList = j.value(uniformLabels.listLabel, json::array());
        if (!uniformList.is_array()) {
            std::cerr << "uniforms entry is not an array" << std::endl;
            return false;
        }

        for (const json& item : uniformList) {
            if (!item.contains(uniformLabels.materialId) || !item.contains(uniformLabels.name) ||
                !item.contains(uniformLabels.type) || !item.contains(uniformLabels.value))
            {
                std::cerr << "uniform entry missing required field(s)" << std::endl;
                return false;
            }

            const unsigned int materialId = item.at(uniformLabels.materialId).get<unsigned int>();
            const std::string name        = item.at(uniformLabels.name).get<std::string>();
            const std::string typeStr     = item.at(uniformLabels.type).get<std::string>();
            const auto typeOpt            = parseUniformType(typeStr);
            if (!typeOpt || *typeOpt == UniformType::NoType) {
                std::cerr << "uniform entry has invalid type: " << typeStr << std::endl;
                return false;
            }

            UniformValue value;
            if (!jsonToUniformValue(item.at(uniformLabels.value), value)) {
                std::cerr << "uniform entry has invalid value object for \"" << name << "\"" << std::endl;
                return false;
            }

            Uniform loaded;
            loaded.name = name;
            loaded.type = *typeOpt;
            loaded.value = value;
            loaded.materialID = materialId;
            loaded.isFunction = item.value(uniformLabels.isFunction, false);
            loaded.isReadOnly = item.value(uniformLabels.isReadOnly, false);
            loaded.useAlternateEditor = item.value(uniformLabels.useAlternateEditor, false);

            const Uniform* existing = uniReg->tryReadUniform(materialId, name);
            if (existing != nullptr && existing->type != loaded.type) {
                uniReg->eraseUniform(materialId, name);
                existing = nullptr;
            }

            if (existing != nullptr) {
                Uniform merged = *existing;
                merged.value = loaded.value;
                merged.isFunction = loaded.isFunction;
                merged.isReadOnly = loaded.isReadOnly;
                merged.useAlternateEditor = loaded.useAlternateEditor;
                uniReg->registerInspectorUniform(materialId, merged);
            }
            else {
                uniReg->registerInspectorUniform(materialId, loaded);
            }
        }
    }
    catch (...) {
        std::cerr << "Error while loading projectJSON (Uniforms)" << std::endl;
        return false;
    }

    return true;
}

bool UniformPersistence::save(const Project& project, json& j) {
    UniformRegistry* uniReg = project.uniformRegistry;
    if (uniReg == nullptr) {
        std::cerr << "Could not save uniforms, uniform registry is null" << std::endl;
        return false;
    }

    j[uniformLabels.listLabel] = json::array();

    for (const auto& [id, uni] : project.uniforms) {
        const Uniform* regUni = uniReg->tryReadUniform(uni.materialID, uni.name);
        if (regUni == nullptr || regUni->ID != id) {
            continue;
        }
        j[uniformLabels.listLabel].push_back({
            { uniformLabels.materialId,          uni.materialID },
            { uniformLabels.name,                uni.name },
            { uniformLabels.type,                to_string(uni.type) },
            { uniformLabels.value,               uniformValueToJson(uni.value) },
            { uniformLabels.isFunction,          uni.isFunction },
            { uniformLabels.isReadOnly,          uni.isReadOnly },
            { uniformLabels.useAlternateEditor,  uni.useAlternateEditor },
        });
    }

    return true;
}
