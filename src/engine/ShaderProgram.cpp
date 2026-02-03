#include "ShaderProgram.hpp"
#include "GetFileContents.hpp"
#include "core/logging/LogSink.hpp"
#include "core/logging/Logger.hpp"


ShaderProgram::ShaderProgram(const char *vertShader_path, const char *fragShader_path, const char *name) : name(name) {
    this->vertPath = std::string(vertShader_path);
    this->fragPath = std::string(fragShader_path);
    vertShader_code = getFileContents(vertShader_path);
    fragShader_code = getFileContents(fragShader_path);


    if (vertShader_code == "" || fragShader_code == "") {
        if (vertShader_code == "") {
            Logger::addLog(LogLevel::CRITICAL, "VERTEX SHADER", "failed to get code from path:", vertShader_path);
        }

        if (fragShader_code == "") {
            Logger::addLog(LogLevel::CRITICAL, "FRAGMENT SHADER", "failed to get code from path:", fragShader_path);
        }
        return;
    }
    if (this->name == "") {
        Logger::addLog(LogLevel::CRITICAL, "ShaderProgram::ShaderProgram", "ShaderProgram created with empty name! Program will crash soon");
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
        Logger::addLog(LogLevel::ERROR, "VERTEX SHADER", "Compilation error:\n", infoLog);
        return;
    }

    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
        Logger::addLog(LogLevel::ERROR, "FRAGMENT SHADER", "Compilation error:\n", infoLog);
        return;
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
        Logger::addLog(LogLevel::ERROR, "SHADER LINK", infoLog);
        return;
    }

    if (success){
        this->m_compiled = true;
    } else {
        char infoLog[512];
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        Logger::addLog(LogLevel::ERROR, "Shader Link Error:\n", infoLog);
        glDeleteProgram(ID);
        this->ID = 0;
        this->m_compiled = false;
        return;
    }
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}


void ShaderProgram::use() {
    if (ID != 0){
        glUseProgram(ID);
    }
}


void ShaderProgram::kill() {
    if (ID!=0){
        glDeleteProgram(ID);
        ID = 0;
    }
}


void ShaderProgram::setUniform_int(const char *uniformName, int val) {
    if (ID == 0) return;
    GLint loc = glGetUniformLocation(ID, uniformName);
    if (loc == -1) {
        Logger::addLog(LogLevel::WARNING, "SHADER UNIFORM: Int", "Location not found for:", uniformName);
        return;
    }
    glUniform1i(loc, val);
}


void ShaderProgram::setUniform_float(const char *uniformName, float val) {
    if (ID == 0) return;
    GLint loc = glGetUniformLocation(ID, uniformName);
    if (loc == -1) {
        Logger::addLog(LogLevel::WARNING, "SHADER UNIFORM Float", "Location not found for:", uniformName);
        return;
    }
    glUniform1f(loc, val);
}


void ShaderProgram::setUniform_vec3int(const char *uniformName, int xVal, int yVal, int zVal) {
    if (ID == 0) return;
    GLint loc = glGetUniformLocation(ID, uniformName);
    if (loc == -1) {
        Logger::addLog(LogLevel::WARNING, "SHADER UNIFORM: Vec3int", "Location not found for:", uniformName);
        return;
    }
    glUniform3i(loc, xVal, yVal, zVal);
}


void ShaderProgram::setUniform_vec3int(const char *uniformName, glm::ivec3 vals) {
    if (ID == 0) return;
    GLint loc = glGetUniformLocation(ID, uniformName);
    if (loc == -1) {
        Logger::addLog(LogLevel::WARNING, "SHADER UNIFORM: Vec3int", "Location not found for:", uniformName);
        return;
    }
    glUniform3i(loc, vals.x, vals.y, vals.z);
}


void ShaderProgram::setUniform_vec3float(const char *uniformName, float xVal, float yVal, float zVal) {
    if (ID == 0) return;
    GLint loc = glGetUniformLocation(ID, uniformName);
    if (loc == -1) {
        Logger::addLog(LogLevel::WARNING, "SHADER UNIFORM: Vec3float", "Location not found for:", uniformName);
        return;
    }
    glUniform3f(loc, xVal, yVal, zVal);
}


void ShaderProgram::setUniform_vec3float(const char *uniformName, glm::fvec3 vals) {
    if (ID == 0) return;
    GLint loc = glGetUniformLocation(ID, uniformName);
    if (loc == -1) {
        Logger::addLog(LogLevel::WARNING, "SHADER UNIFORM: Vec3float", "Location not found for:", uniformName);
        return;
    }
    glUniform3f(loc, vals.x, vals.y, vals.z);
}

void ShaderProgram::setUniform_vec4float(const char *uniformName, glm::fvec4 vals) {
    if (ID == 0) return;
    GLint loc = glGetUniformLocation(ID, uniformName);
    if (loc == -1) {
        Logger::addLog(LogLevel::WARNING, "SHADER UNIFORM: Vec3float", "Location not found for:", uniformName);
        return;
    }
    glUniform3f(loc, vals.x, vals.y, vals.z);
}


void ShaderProgram::setUniform_mat4float(const char *uniformName, glm::fmat4 M) const {
    if (ID == 0) return;
    GLint loc = glGetUniformLocation(ID, uniformName);
    if (loc == -1) {
        Logger::addLog(LogLevel::WARNING, "SHADER UNIFORM: mat4float", "Location not found for:", uniformName);
        return;
    }
    glUniformMatrix4fv(loc, 1, GL_FALSE, &M[0][0]);
}

glm::vec3 ShaderProgram::getUniform_vec3float(const char* uniformName) {
    if (ID == 0) return glm::vec3(0);
    GLint loc = glGetUniformLocation(ID, uniformName);
    if (loc == -1) {
        Logger::addLog(LogLevel::WARNING, "SHADER UNIFORM: Vec3float", "Location not found for:", uniformName);
        return glm::vec3(0);
    }

    GLfloat value[3];
    glGetUniformfv(ID, loc, value);
    return glm::vec3(value[0], value[1], value[2]);
}

glm::vec4 ShaderProgram::getUniform_vec4float(const char* uniformName) {
    if (ID == 0) return glm::vec4(0);
    GLint loc = glGetUniformLocation(ID, uniformName);
    if (loc == -1) {
        Logger::addLog(LogLevel::WARNING, "SHADER UNIFORM: Vec4float", "Location not found for:", uniformName);
        return glm::vec4(0);
    }

    GLfloat value[4];
    glGetUniformfv(ID, loc, value); 
    return glm::vec4(value[0], value[1], value[2], value[3]);
}

float ShaderProgram::getUniform_float(const char* uniformName) {
    if (ID == 0) return 0.0f;
    GLint loc = glGetUniformLocation(ID, uniformName);
    if (loc == -1) {
        Logger::addLog(LogLevel::WARNING, "SHADER UNIFORM: Vec4float", "Location not found for:", uniformName);
        return 0;
    }

    GLfloat value[1];
    glGetUniformfv(ID, loc, value); 
    return value[0];
}

int ShaderProgram::getUniform_int(const char* uniformName) {
    if (ID == 0) return 0;
    GLint loc = glGetUniformLocation(ID, uniformName);
    if (loc == -1) {
        Logger::addLog(LogLevel::WARNING, "SHADER UNIFORM: Vec4float", "Location not found for:", uniformName);
        return 0;
    }

    GLint value[1];
    glGetUniformiv(ID, loc, value); 
    return value[0];
}

bool ShaderProgram::hasUniform(const char* uniformName) {
    if (ID == 0) return false;
    GLint loc = glGetUniformLocation(ID, uniformName);
    if (loc == -1) {
        return false;
    }
    else return true;

}

ShaderProgram::~ShaderProgram(){
    this->kill();
}
