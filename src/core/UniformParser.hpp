#pragma once 

#include <unordered_map>
#include "UniformTypes.hpp"
#include "ShaderRegistry.hpp"
#include "logging/Logger.hpp"

class UniformParser {
public :
    UniformParser(Logger* _loggerPtr);
    std::unordered_map<std::string, Uniform> parseUniforms(const ShaderProgram& program);

private:
    struct UniformInStruct {
        std::string uniformName;
        std::string typeName; // We have to store type as a name because type could be a custom struct.
    };
    enum class ParseState { Default, StructDefinition, UniformDeclaration };
    enum class LastTokenWas { IgnoreLastToken, Uniform, StructName, TypeName, UniformName, SemiColon, Comma, Struct, LeftSqrBracket, RightSqrBracket, RightCurlyBrace, LeftCurlyBrace};

    std::vector<std::string> tokenizeShaderCode(const ShaderProgram& program);
    void handleUniformName(
        std::unordered_map<std::string, Uniform>& programUniforms,
        const std::string& uniformName,
        const std::string& typeName,
        std::unordered_map<std::string, std::vector<UniformInStruct>>& structDefinitions,
        const std::vector<std::string>& tokens,
        int tokenIndex,
        const std::string& structName = ""   // optional: "" at top level, parent name when recursing
    );
    Logger* loggerPtr;
};
