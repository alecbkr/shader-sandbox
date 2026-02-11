#include "SettingsLoader.hpp"

#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include "application/AppContext.hpp"

using json = nlohmann::json;

int SettingsLoader::version = 1;

bool SettingsLoader::load(AppSettings& settings) {
    if (!std::filesystem::exists(settings.settingsPath)) {
        return false;
    }

    std::ifstream in(settings.settingsPath);
    if (!in.is_open()) {
        return false;
    }

    try {
        json j;
        in >> j;

        SettingsLoader::version = j.value("version", 1);
        settings.width = j.value("windowWidth",  settings.width);
        settings.height = j.value("windowHeight", settings.height);
        settings.posX = j.value("windowPositionX", settings.posX);
        settings.posY = j.value("windowPositionY", settings.posY);
    } catch (...) {
        // malformed JSON -> fall back to defaults
        return false;
    }

    return true;
}

void SettingsLoader::save(const AppSettings& settings) {
    std::filesystem::create_directories(settings.settingsPath.parent_path());

    json j;
    j["version"] = version;
    j["windowWidth"] = settings.width;
    j["windowHeight"] = settings.height;
    j["windowPositionX"] = settings.posX;
    j["windowPositionY"] = settings.posY;

    std::ofstream out(settings.settingsPath);
    out << j.dump(4);
}
