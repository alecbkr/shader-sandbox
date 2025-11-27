#include "ShaderProgram.hpp"

#include "GetFileContents.hpp"
#include "Errorlog.hpp"



ShaderProgram::ShaderProgram(const char *vertShader_path, const char *fragShader_path) {
    std::string vertShader_code = getFileContents(vertShader_path);
    std::string fragShader_code = getFileContents(fragShader_path);

    if (vertShader_code == "" || fragShader_code == "") {
        if (vertShader_code == "") {
            ERRLOG.logEntry(critical, "VERTEX SHADER", "failed to get code from path:", vertShader_path);
        }

        if (fragShader_code == "") {
            ERRLOG.logEntry(critical, "FRAGMENT SHADER", "failed to get code from path:", fragShader_path);
        }
        return;
    }
    const char *vertShader_src = vertShader_code.c_str();
    const char *fragShader_src = fragShader_code.c_str();
   
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertShader_src, NULL);
    glCompileShader(vertShader);

    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragShader_src, NULL);
    glCompileShader(fragShader);

    
    // Check for compilation errors
    GLint success;
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertShader, 512, NULL, infoLog);
        ERRLOG.logEntry(critical, "VERTEX SHADER", "Compilation error:\n", infoLog);
    }

    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
        ERRLOG.logEntry(critical, "FRAGMENT SHADER", "Compilation error:\n", infoLog);
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertShader);
    glAttachShader(ID, fragShader);
    glLinkProgram(ID);

    // Check if link was successful
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        ERRLOG.logEntry(critical, "SHADER", infoLog);
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}

void ShaderProgram::use() {
    glUseProgram(ID);
}

void ShaderProgram::kill() {
    glDeleteProgram(ID);
}


void ShaderProgram::setFloat(const char *uniformName, float val) {
    GLint loc = glGetUniformLocation(ID, uniformName);
    if (loc == -1) {
        ERRLOG.logEntry(warning, "SHADER UNIFORM Float", "Location not found for:", uniformName);
        return;
    }
    glUniform1f(loc, val);
}


void ShaderProgram::setVec3f(const char *uniformName, float xVal, float yVal, float zVal) {
    GLint loc = glGetUniformLocation(ID, uniformName);
    if (loc == -1) {
        ERRLOG.logEntry(warning, "SHADER UNIFORM Vec3f", "Location not found for:", uniformName);
        return;
    }
    glUniform3f(loc, xVal, yVal, zVal);
}


void ShaderProgram::setVec3i(const char *uniformName, int xVal, int yVal, int zVal) {
    GLint loc = glGetUniformLocation(ID, uniformName);
    if (loc == -1) {
        ERRLOG.logEntry(warning, "SHADER UNIFORM Vec3i", "Location not found for:", uniformName);
        return;
    }
    glUniform3i(loc, xVal, yVal, zVal);
}