#include "UniformParser.hpp"
#include "core/UniformTypes.hpp"
#include "core/logging/LogSink.hpp"
#include <sys/stat.h>
#include <vector>
#include <stack>

UniformParser::UniformParser(Logger* _loggerPtr) {
    loggerPtr = _loggerPtr;
}

std::unordered_map<std::string, Uniform> UniformParser::parseUniforms(const ShaderProgram& program) {
    std::unordered_map<std::string, Uniform> programUniforms;
    if (!program.isCompiled()) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "parseUniforms", "should not be parsing an invalid shader program!");
        return programUniforms;
    }
    // This code assumes the shader file is valid, and thus doesn't check syntax

    std::vector<std::string> tokens = tokenizeShaderCode(program);
    std::unordered_map<std::string, std::vector<UniformInStruct>> structDefinitions;
    std::string currentStructName;
    std::string typeName;
    
    LastTokenWas lastTokenWas = LastTokenWas::IgnoreLastToken;
    ParseState state = ParseState::Default;
    for (int i = 0; i < tokens.size(); i++) {
        std::string& token = tokens[i];
        switch (lastTokenWas) {
            case LastTokenWas::IgnoreLastToken: {
                if (token == "uniform") {
                    lastTokenWas = LastTokenWas::Uniform;
                    state = ParseState::UniformDeclaration;
                }
                else if (token == "struct") {
                    lastTokenWas = LastTokenWas::Struct;
                    state = ParseState::StructDefinition;
                }
                break;
            }
            case LastTokenWas::Uniform: {
                // Read type name.
                if (!glslTypeMap.contains(token) && !structDefinitions.contains(token)) {
                    loggerPtr->addLog(LogLevel::WARNING, "parseUnifoms", "Uniform parser error while parsing type name: ", token); 
                    return programUniforms;
                }

                typeName = token;
                lastTokenWas = LastTokenWas::TypeName;
                break;
            }
            case LastTokenWas::TypeName: {
                // If we read a type, at this point the only thing we can do is read the name of a variable.
                std::string& uniformName = token;
                if (state == ParseState::StructDefinition) {
                    handleUniformName(programUniforms, uniformName, typeName, structDefinitions, tokens, i, currentStructName);
                }
                else if (state == ParseState::UniformDeclaration) {
                    handleUniformName(programUniforms, uniformName, typeName, structDefinitions, tokens, i);
                }
                else {
                    loggerPtr->addLog(LogLevel::LOG_ERROR, "UniformParser::parseUniforms", "How did you get here? this should be unreachable (last token was a uniform type)");
                }
                
                lastTokenWas = LastTokenWas::UniformName;
                break;
            }
            case LastTokenWas::UniformName: {
                /* At this point, we go to 
                    [ for array -> num (could be a define too, gotta implement that -> ] -> comma or SemiColon
                    , -> next uniformName
                    ; -> go back to nothing cause it's a new statement
                */
                
                if (token == "[") {
                    // hanlded in handleUniformName, skip the number & "]" tokens.
                    i += 2;
                }
                else if (token == ";") {
                    if (state == ParseState::UniformDeclaration) {
                        state = ParseState::Default;

                        // Rare case where we reset things
                        lastTokenWas = LastTokenWas::IgnoreLastToken;
                    }
                    else if (state == ParseState::StructDefinition) {
                        lastTokenWas = LastTokenWas::SemiColon;
                    }
                }
                else if (token == ",") {
                    lastTokenWas = LastTokenWas::Comma;
                }
                else {
                    loggerPtr->addLog(LogLevel::LOG_ERROR, "UniformParser::parseUniforms", "How did you get here? this should be unreachable (last token was a uniform name)");
                }
                break;
            }
            case LastTokenWas::SemiColon: {
                if (state == ParseState::StructDefinition) {
                    if (token == "}") {
                        state = ParseState::Default;
                        lastTokenWas = LastTokenWas::IgnoreLastToken;
                    } 
                    else if (glslTypeMap.contains(token) || structDefinitions.contains(token)) {
                        typeName = token;
                        lastTokenWas = LastTokenWas::TypeName;
                    } 
                    else {
                        loggerPtr->addLog(LogLevel::LOG_ERROR, "parseUniforms", "expected '}' or type in struct, got: " + token);
                        return programUniforms;
                    }
    }
                else 
                    loggerPtr->addLog(LogLevel::LOG_ERROR, "UniformParser::parseUniforms", "How did you get here? (last token was a semicolon)");
                break;
            }
            case LastTokenWas::Comma: {
                if (state == ParseState::UniformDeclaration) {
                    std::string& uniformName = token;
                    handleUniformName(programUniforms, uniformName, typeName, structDefinitions, tokens, i);
                    lastTokenWas = LastTokenWas::UniformName;
                }
                else {
                    loggerPtr->addLog(LogLevel::LOG_ERROR, "UniformParser::parseUniforms", "something went wrong");
                }

                break;
            }
            case LastTokenWas::Struct: {
                currentStructName = token;
                structDefinitions[currentStructName] = {};
                lastTokenWas = LastTokenWas::StructName;
                
                break;
            }
            case LastTokenWas::StructName: {
                if (token != "{") {
                    loggerPtr->addLog(LogLevel::LOG_ERROR, "parseUniforms", "expected '{' after struct name");
                    return programUniforms;
                }
                lastTokenWas = LastTokenWas::LeftCurlyBrace;

                break;
            }
            case LastTokenWas::LeftCurlyBrace: {
                // opening up the struct definition. this token is either ending struct (empty struct)
                // or we're reading a type name
                if (token == "}") {
                    lastTokenWas = LastTokenWas::IgnoreLastToken;
                    state = ParseState::Default;
                }
                typeName = token;
                lastTokenWas = LastTokenWas::TypeName;
                break;
            }
        }
    }

    return programUniforms;
}

std::vector<std::string> UniformParser::tokenizeShaderCode(const ShaderProgram& program) {
    // Cursor wrote this, it seemed good and caught a lot of stuff I wouldn't have first try.
    std::string source = program.vertShader_code + "\n" + program.fragShader_code;
    std::vector<std::string> tokens;
    tokens.reserve(source.size() / 4u); 

    enum { Normal, LineComment, BlockComment, DoubleQuotedString, SingleQuotedString } state = Normal;
    std::string currentToken;

    for (size_t i = 0; i < source.size(); ) {
        char currentChar = source[i];
        char nextChar = (i + 1 < source.size()) ? source[i + 1] : '\0';

        switch (state) {
        case Normal: {
            if (std::isspace(static_cast<unsigned char>(currentChar))) {
                i++;
                continue;
            }
            if (currentChar == '/' && nextChar == '/') { state = LineComment; i += 2; continue; }
            if (currentChar == '/' && nextChar == '*') { state = BlockComment; i += 2; continue; }
            if (currentChar == '"') { state = DoubleQuotedString; i++; continue; }
            if (currentChar == '\'') { state = SingleQuotedString; i++; continue; }

            if (std::isalpha(static_cast<unsigned char>(currentChar)) || currentChar == '_') {
                currentToken.clear();
                while (i < source.size() && (std::isalnum(static_cast<unsigned char>(source[i])) || source[i] == '_'))
                    currentToken += source[i++];
                tokens.push_back(currentToken);
                continue;
            }
            if (std::isdigit(static_cast<unsigned char>(currentChar)) || (currentChar == '.' && i + 1 < source.size() && std::isdigit(static_cast<unsigned char>(source[i + 1])))) {
                currentToken.clear();
                while (i < source.size() && (std::isdigit(static_cast<unsigned char>(source[i])) || source[i] == '.' || source[i] == 'e' || source[i] == 'E' || source[i] == '-' || source[i] == '+'))
                    currentToken += source[i++];
                tokens.push_back(currentToken);
                continue;
            }
            if (currentChar == ';' || currentChar == ',' || currentChar == '[' || currentChar == ']' || currentChar == '{' || currentChar == '}' || currentChar == '(' || currentChar == ')') {
                tokens.push_back(std::string(1, currentChar));
                i++;
                continue;
            }
            i++;
            break;
        } case LineComment: {
            if (currentChar == '\n') state = Normal;
            i++;
            break;
        } case BlockComment: {
            if (currentChar == '*' && nextChar == '/') { state = Normal; i += 2; continue; }
            i++;
            break;
        } case DoubleQuotedString: case SingleQuotedString: {
            if (currentChar == '\\') { i += 2; continue; }
            if ((state == DoubleQuotedString && currentChar == '"') || (state == SingleQuotedString && currentChar == '\'')) state = Normal;
            i++;
            break;
        }
        }
    }

    return tokens;
}

void UniformParser::handleUniformName(
    std::unordered_map<std::string, Uniform>& programUniforms,
    const std::string& uniformName,
    const std::string& typeName,
    std::unordered_map<std::string, std::vector<UniformInStruct>>& structDefinitions,
    const std::vector<std::string>& tokens,
    int tokenIndex,
    const std::string& structName   // optional: "" at top level, parent name when recursing
) {
    ParseState state;
    // structName is "" by default
    if (structName.empty()) {
        state = ParseState::UniformDeclaration;
    }
    else state = ParseState::StructDefinition;

    // rare case where we just handle tokens manually.
    bool isArray = tokenIndex < tokens.size() - 4 && tokens[tokenIndex + 1] == "[";
    bool isStruct = structDefinitions.contains(typeName);

    // base case: simple glsl uniform.
    if (!isStruct && !isArray) {
        if (!glslTypeMap.contains(typeName)) {
            loggerPtr->addLog(LogLevel::LOG_ERROR, "UniformParser::appendUniform", "type " + typeName + " not supported yet");
            return;
        }
        if (state == ParseState::UniformDeclaration) {
            UniformType type = glslTypeMap.at(typeName); 
            programUniforms[uniformName] = Uniform{.name = uniformName, .type = type};
        }
        else if (state == ParseState::StructDefinition) {
            std::vector<UniformInStruct>& uniformsInStruct = structDefinitions.at(structName);
            uniformsInStruct.push_back(UniformInStruct{.uniformName = uniformName, .typeName = typeName});
        }
        return;
    }

    // Handle array
    if (isArray) {
        const std::string& arraySizeToken = tokens[tokenIndex + 2];
        const std::string& rightBracket = tokens[tokenIndex + 3];

        bool isDigit = true;
        for (char a : arraySizeToken) {
            if (!std::isdigit(a)) {
                isDigit = false;
            }
        }
        
        int arraySize;
        if (isDigit) {
            arraySize = std::atoi(arraySizeToken.c_str());
        }
        else {
            loggerPtr->addLog(LogLevel::LOG_ERROR, "UniformParser::parseUniforms", "Can't handle array sizes with defines yet!");
            return;
        }

        
        for (int i = 0; i< arraySize; i++) {
            std::string nameToUse = uniformName + "[" + std::to_string(i) + "]";
            handleUniformName(programUniforms, nameToUse, typeName, structDefinitions, tokens, tokenIndex + 4, structName);
        }
        return;
    }

    // Handle struct 
    // Recursion, yay. 
    // Might be better to use a stack based approach, but this is fine for now. It might fail under 20+ nested structs, 
    // but who is doing that???
    if (!isStruct) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "UniformParser::parseUniforms", "Error: on struct path but not handling a struct...");
        return;
    }
    std::vector<UniformInStruct> structUniforms = structDefinitions.at(typeName);
    for (UniformInStruct uni : structUniforms) {
        std::string uniName = uniformName + '.' + uni.uniformName;
        handleUniformName(programUniforms, uniName, uni.typeName, structDefinitions, tokens, tokenIndex, structName);
    }
    return;
}
