#include "ShaderProgram.hpp"
#include "GetFileContents.hpp"
#include "core/logging/LogSink.hpp"
#include "core/logging/Logger.hpp"


ShaderProgram::ShaderProgram(const char *vertShader_path, const char *fragShader_path, const char *name, const unsigned int ID, Logger* _loggerPtr) : name(name), ID(ID), loggerPtr(_loggerPtr) {
    this->vertPath = std::string(vertShader_path);
    this->fragPath = std::string(fragShader_path);
    vertShader_code = getFileContents(vertShader_path);
    fragShader_code = getFileContents(fragShader_path);


    if (vertShader_code == "" || fragShader_code == "") {
        if (vertShader_code == "") {
            loggerPtr->addLog(LogLevel::CRITICAL, "VERTEX SHADER", "failed to get code from path:", vertShader_path);
        }

        if (fragShader_code == "") {
            loggerPtr->addLog(LogLevel::CRITICAL, "FRAGMENT SHADER", "failed to get code from path:", fragShader_path);
        }
        return;
    }
    if (this->name == "") {
        loggerPtr->addLog(LogLevel::CRITICAL, "ShaderProgram::ShaderProgram", "ShaderProgram created with empty name! Program will crash soon");
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
        loggerPtr->addLog(LogLevel::LOG_ERROR, "VERTEX SHADER", "Compilation error:\n", infoLog);
        return;
    }

    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
        loggerPtr->addLog(LogLevel::LOG_ERROR, "FRAGMENT SHADER", "Compilation error:\n", infoLog);
        return;
    }

    gpuID = glCreateProgram();
    glAttachShader(gpuID, vertShader);
    glAttachShader(gpuID, fragShader);
    glLinkProgram(gpuID);

    // Check if link was successful
    glGetProgramiv(gpuID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(gpuID, 512, NULL, infoLog);
        loggerPtr->addLog(LogLevel::LOG_ERROR, "SHADER LINK", infoLog);
        return;
    }

    if (success){
        this->m_compiled = true;
    } else {
        char infoLog[512];
        glGetProgramInfoLog(gpuID, 512, NULL, infoLog);
        loggerPtr->addLog(LogLevel::LOG_ERROR, "Shader Link Error:\n", infoLog);
        glDeleteProgram(gpuID);
        this->gpuID = 0;
        this->m_compiled = false;
        return;
    }
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}


void ShaderProgram::use() {
    if (gpuID != 0){
        glUseProgram(gpuID);
    }
}


void ShaderProgram::kill() {
    if (gpuID!=0){
        glDeleteProgram(gpuID);
        gpuID = 0;
    }
}


void ShaderProgram::setUniform_int(const char *uniformName, int val) {
    if (gpuID == 0) return;
    GLint loc = glGetUniformLocation(gpuID, uniformName);
    if (loc == -1) {
        loggerPtr->addLog(LogLevel::WARNING, "SHADER UNIFORM: Int", "Location not found for:", uniformName);
        return;
    }
    glUniform1i(loc, val);
}


void ShaderProgram::setUniform_float(const char *uniformName, float val) {
    if (gpuID == 0) return;
    GLint loc = glGetUniformLocation(gpuID, uniformName);
    if (loc == -1) {
        loggerPtr->addLog(LogLevel::WARNING, "SHADER UNIFORM Float", "Location not found for:", uniformName);
        return;
    }
    glUniform1f(loc, val);
}


void ShaderProgram::setUniform_vec3int(const char *uniformName, int xVal, int yVal, int zVal) {
    if (gpuID == 0) return;
    GLint loc = glGetUniformLocation(gpuID, uniformName);
    if (loc == -1) {
        loggerPtr->addLog(LogLevel::WARNING, "SHADER UNIFORM: Vec3int", "Location not found for:", uniformName);
        return;
    }
    glUniform3i(loc, xVal, yVal, zVal);
}


void ShaderProgram::setUniform_vec3int(const char *uniformName, glm::ivec3 vals) {
    if (gpuID == 0) return;
    GLint loc = glGetUniformLocation(gpuID, uniformName);
    if (loc == -1) {
        loggerPtr->addLog(LogLevel::WARNING, "SHADER UNIFORM: Vec3int", "Location not found for:", uniformName);
        return;
    }
    glUniform3i(loc, vals.x, vals.y, vals.z);
}


void ShaderProgram::setUniform_vec3float(const char *uniformName, float xVal, float yVal, float zVal) {
    if (gpuID == 0) return;
    GLint loc = glGetUniformLocation(gpuID, uniformName);
    if (loc == -1) {
        loggerPtr->addLog(LogLevel::WARNING, "SHADER UNIFORM: Vec3float", "Location not found for:", uniformName);
        return;
    }
    glUniform3f(loc, xVal, yVal, zVal);
}


void ShaderProgram::setUniform_vec3float(const char *uniformName, glm::fvec3 vals) {
    if (gpuID == 0) return;
    GLint loc = glGetUniformLocation(gpuID, uniformName);
    if (loc == -1) {
        loggerPtr->addLog(LogLevel::WARNING, "SHADER UNIFORM: Vec3float", "Location not found for:", uniformName);
        return;
    }
    glUniform3f(loc, vals.x, vals.y, vals.z);
}

void ShaderProgram::setUniform_vec4float(const char *uniformName, glm::fvec4 vals) {
    if (gpuID == 0) return;
    GLint loc = glGetUniformLocation(gpuID, uniformName);
    if (loc == -1) {
        loggerPtr->addLog(LogLevel::WARNING, "SHADER UNIFORM: Vec4float", "Location not found for:", uniformName);
        return;
    }
    glUniform4f(loc, vals.x, vals.y, vals.z, vals.w);
}


void ShaderProgram::setUniform_mat4float(const char *uniformName, glm::fmat4 M) const {
    if (gpuID == 0) return;
    GLint loc = glGetUniformLocation(gpuID, uniformName);
    if (loc == -1) {
        loggerPtr->addLog(LogLevel::WARNING, "SHADER UNIFORM: mat4float", "Location not found for:", uniformName);
        return;
    }
    glUniformMatrix4fv(loc, 1, GL_FALSE, &M[0][0]);
}

glm::vec3 ShaderProgram::getUniform_vec3float(const char* uniformName) {
    if (gpuID == 0) return glm::vec3(0);
    GLint loc = glGetUniformLocation(gpuID, uniformName);
    if (loc == -1) {
        loggerPtr->addLog(LogLevel::WARNING, "SHADER UNIFORM: Vec3float", "Location not found for:", uniformName);
        return glm::vec3(0);
    }

    GLfloat value[3];
    glGetUniformfv(gpuID, loc, value);
    return glm::vec3(value[0], value[1], value[2]);
}

glm::vec4 ShaderProgram::getUniform_vec4float(const char* uniformName) {
    if (gpuID == 0) return glm::vec4(0);
    GLint loc = glGetUniformLocation(gpuID, uniformName);
    if (loc == -1) {
        loggerPtr->addLog(LogLevel::WARNING, "SHADER UNIFORM: Vec4float", "Location not found for:", uniformName);
        return glm::vec4(0);
    }

    GLfloat value[4];
    glGetUniformfv(gpuID, loc, value); 
    return glm::vec4(value[0], value[1], value[2], value[3]);
}

float ShaderProgram::getUniform_float(const char* uniformName) {
    if (gpuID == 0) return 0.0f;
    GLint loc = glGetUniformLocation(gpuID, uniformName);
    if (loc == -1) {
        loggerPtr->addLog(LogLevel::WARNING, "SHADER UNIFORM: Vec4float", "Location not found for:", uniformName);
        return 0;
    }

    GLfloat value[1];
    glGetUniformfv(gpuID, loc, value); 
    return value[0];
}

int ShaderProgram::getUniform_int(const char* uniformName) {
    if (gpuID == 0) return 0;
    GLint loc = glGetUniformLocation(gpuID, uniformName);
    if (loc == -1) {
        loggerPtr->addLog(LogLevel::WARNING, "SHADER UNIFORM: Vec4float", "Location not found for:", uniformName);
        return 0;
    }

    GLint value[1];
    glGetUniformiv(gpuID, loc, value); 
    return value[0];
}

bool ShaderProgram::hasUniform(const char* uniformName) {
    if (gpuID == 0) return false;
    GLint loc = glGetUniformLocation(gpuID, uniformName);
    if (loc == -1) {
        return false;
    }
    else return true;

}

ShaderProgram::~ShaderProgram(){
    this->kill();
}
