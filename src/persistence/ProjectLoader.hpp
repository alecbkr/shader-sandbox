#pragma once
#include <string>

struct Project;

struct ProjectLoader {
    static int version;

    static bool load(Project& project);
    static void save(const Project& project);
};

struct ShaderData {
    bool isCompiled;
    std::string name;
    std::string vert_path;
    std::string frag_path;
};

struct UniformData {

};
