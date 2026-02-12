#include "ProjectLoader.hpp"

#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include "application/AppContext.hpp"

using json = nlohmann::json;

int ProjectLoader::version = 1;

bool ProjectLoader::load(Project& project) {
    if (!std::filesystem::exists(project.projectJSON)) 
    {
        std::cerr << "projectJSON does not exist" << std::endl;
        return false;
    }

    std::ifstream in(project.projectJSON);
    if (!in.is_open()) {
        std::cerr << "Could not open projectJSON" << std::endl;
        return false;
    }

    try {
        json j;
        in >> j;

        ProjectLoader::version = j.value("version", 1);
        project.projectTitle = j.value("projectTitle", project.projectTitle);
    } catch (...) {
        std::cerr << "Error while loading projectJSON" << std::endl;
        return false;
    }

    return true;
}

void ProjectLoader::save(const Project& project) {
    std::filesystem::create_directories(project.projectRoot);
    std::filesystem::create_directories(project.projectShadersDir);

    json j;
    j["version"] = version;
    j["projectTitle"] = project.projectTitle;

    std::ofstream out(project.projectJSON);
    out << j.dump(4);
}
