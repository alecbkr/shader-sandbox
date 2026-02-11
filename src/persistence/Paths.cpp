#include "Paths.hpp"
#include <cstdlib>
#include <stdexcept>

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

}
