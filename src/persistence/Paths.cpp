#include "Paths.hpp"
#include <cstdlib>
#include <stdexcept>
#include <cstring>

namespace Paths {

std::filesystem::path getUserConfigDir(const std::string& appName) {
    namespace fs = std::filesystem;

#ifdef _WIN32
    const char* appData = std::getenv("APPDATA");
    if (!appData) throw std::runtime_error("APPDATA not set");
    fs::path dir = fs::path(appData) / appName;

#else // Linux
    const char* xdg = std::getenv("XDG_CONFIG_HOME");
    fs::path base;

    if (xdg) base = xdg;
    else {
        const char* home = std::getenv("HOME");
        if (!home) throw std::runtime_error("HOME not set");
        base = fs::path(home) / ".config";
    }

    fs::path dir = base / appName;
#endif

    fs::create_directories(dir);
    return dir;
}

std::filesystem::path getProjectRootDir(std::string projectToOpen, std::string& projectTitle) {
    namespace fs = std::filesystem;

#ifdef _WIN32
    const char* userProfile = std::getenv("USERPROFILE");
    if (!userProfile) throw std::runtime_error("USERPROFILE not set");
    fs::path documents = fs::path(userProfile) / "Documents";
#else
    const char* home = std::getenv("HOME");
    if (!home) throw std::runtime_error("HOME not set");
    fs::path documents = fs::path(home) / "Documents";
#endif

    fs::path prismRoot = documents / "PrismTSS";

    // Determine project name
    std::string projectName;
    if (!projectToOpen.empty() && std::filesystem::exists(prismRoot / projectToOpen)) {
        projectName = projectToOpen;
    } else {
        int i = 1;
        std::string newName = "Untitled(" + std::to_string(i) + ")";

        if (!std::filesystem::exists(prismRoot / "Untitled")) {
            projectName = "Untitled";
        } else {
            while (std::filesystem::exists(prismRoot/ newName)) {
                newName = "Untitled(" + std::to_string(i) + ")";
                i++;
            }

            projectName = newName;
        }
    }

    projectTitle = projectName;
    fs::path projectRoot = prismRoot / projectName;

    // Ensure directory exists
    fs::create_directories(projectRoot);
    fs::create_directories(projectRoot / "shaders");

    return projectRoot;
}

}
