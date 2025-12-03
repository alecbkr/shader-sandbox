#include "ShaderProgram.hpp"

#include "GetFileContents.hpp"
#include "Errorlog.hpp"


ShaderProgram::ShaderProgram(const char *vertShader_path, const char *fragShader_path) {
    vertShader_code = getFileContents(vertShader_path);
    fragShader_code = getFileContents(fragShader_path);


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
        ERRLOG.logEntry(critical, "SHADER LINK", infoLog);
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


void ShaderProgram::setUniform_int(const char *uniformName, int val) {
    GLint loc = glGetUniformLocation(ID, uniformName);
    if (loc == -1) {
        ERRLOG.logEntry(warning, "SHADER UNIFORM: Int", "Location not found for:", uniformName);
        return;
    }
    glUniform1f(loc, val);
}


void ShaderProgram::setUniform_float(const char *uniformName, float val) {
    GLint loc = glGetUniformLocation(ID, uniformName);
    if (loc == -1) {
        ERRLOG.logEntry(warning, "SHADER UNIFORM Float", "Location not found for:", uniformName);
        return;
    }
    glUniform1f(loc, val);
}


void ShaderProgram::setUniform_vec3int(const char *uniformName, int xVal, int yVal, int zVal) {
    GLint loc = glGetUniformLocation(ID, uniformName);
    if (loc == -1) {
        ERRLOG.logEntry(warning, "SHADER UNIFORM: Vec3int", "Location not found for:", uniformName);
        return;
    }
    glUniform3i(loc, xVal, yVal, zVal);
}


void ShaderProgram::setUniform_vec3int(const char *uniformName, glm::ivec3 vals) {
    GLint loc = glGetUniformLocation(ID, uniformName);
    if (loc == -1) {
        ERRLOG.logEntry(warning, "SHADER UNIFORM: Vec3int", "Location not found for:", uniformName);
        return;
    }
    glUniform3i(loc, vals.x, vals.y, vals.z);
}


void ShaderProgram::setUniform_vec3float(const char *uniformName, float xVal, float yVal, float zVal) {
    GLint loc = glGetUniformLocation(ID, uniformName);
    if (loc == -1) {
        ERRLOG.logEntry(warning, "SHADER UNIFORM: Vec3float", "Location not found for:", uniformName);
        return;
    }
    glUniform3f(loc, xVal, yVal, zVal);
}


void ShaderProgram::setUniform_vec3float(const char *uniformName, glm::fvec3 vals) {
    GLint loc = glGetUniformLocation(ID, uniformName);
    if (loc == -1) {
        ERRLOG.logEntry(warning, "SHADER UNIFORM: Vec3float", "Location not found for:", uniformName);
        return;
    }
    glUniform3f(loc, vals.x, vals.y, vals.z);
}

void ShaderProgram::setUniform_vec4float(const char *uniformName, glm::fvec4 vals) {
    GLint loc = glGetUniformLocation(ID, uniformName);
    if (loc == -1) {
        ERRLOG.logEntry(warning, "SHADER UNIFORM: Vec3float", "Location not found for:", uniformName);
        return;
    }
    glUniform3f(loc, vals.x, vals.y, vals.z);
}


void ShaderProgram::setUniform_mat4float(const char *uniformName, glm::fmat4 M) const {
    GLint loc = glGetUniformLocation(ID, uniformName);
    if (loc == -1) {
        ERRLOG.logEntry(warning, "SHADER UNIFORM: mat4float", "Location not found for:", uniformName);
        return;
    }
    glUniformMatrix4fv(loc, 1, GL_FALSE, &M[0][0]);
}

glm::vec3 ShaderProgram::getUniform_vec3float(const char* uniformName) {
    GLint loc = glGetUniformLocation(ID, uniformName);
    if (loc == -1) {
        ERRLOG.logEntry(warning, "SHADER UNIFORM: Vec3float", "Location not found for:", uniformName);
        return glm::vec3(0);
    }

    GLfloat value[3];
    glGetUniformfv(ID, loc, value);
    return glm::vec3(value[0], value[1], value[2]);
}

glm::vec4 ShaderProgram::getUniform_vec4float(const char* uniformName) {
    GLint loc = glGetUniformLocation(ID, uniformName);
    if (loc == -1) {
        ERRLOG.logEntry(warning, "SHADER UNIFORM: Vec4float", "Location not found for:", uniformName);
        return glm::vec4(0);
    }

    GLfloat value[4];
    glGetUniformfv(ID, loc, value); 
    return glm::vec4(value[0], value[1], value[2], value[3]);
}

float ShaderProgram::getUniform_float(const char* uniformName) {
    GLint loc = glGetUniformLocation(ID, uniformName);
    if (loc == -1) {
        ERRLOG.logEntry(warning, "SHADER UNIFORM: Vec4float", "Location not found for:", uniformName);
        return 0;
    }

    GLfloat value[1];
    glGetUniformfv(ID, loc, value); 
    return value[0];
}

int ShaderProgram::getUniform_int(const char* uniformName) {
    GLint loc = glGetUniformLocation(ID, uniformName);
    if (loc == -1) {
        ERRLOG.logEntry(warning, "SHADER UNIFORM: Vec4float", "Location not found for:", uniformName);
        return 0;
    }

    GLint value[1];
    glGetUniformiv(ID, loc, value); 
    return value[0];
}

bool ShaderProgram::hasUniform(const char* uniformName) {
    GLint loc = glGetUniformLocation(ID, uniformName);
    if (loc == -1) {
        return false;
    }
    else return true;

}
