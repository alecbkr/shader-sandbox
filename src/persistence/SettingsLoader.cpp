#include "SettingsLoader.hpp"

#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include "application/AppContext.hpp"

using json = nlohmann::json;

int SettingsLoader::version = 1;

bool SettingsLoader::load(AppSettings& settings) {
    if (!std::filesystem::exists(settings.settingsPath)) return false;

    std::ifstream in(settings.settingsPath);
    if (!in.is_open()) return false;

    try {
        json j;
        in >> j;

        SettingsLoader::version = j.value("version", 1);
        
        // Load window information
        settings.width = j.value("windowWidth",  settings.width);
        settings.height = j.value("windowHeight", settings.height);
        settings.posX = j.value("windowPositionX", settings.posX);
        settings.posY = j.value("windowPositionY", settings.posY);

        // Load keybinds
        if (j.contains("keybinds") && j["keybinds"].is_object()) {
            const auto& saved_kb = j["keybinds"];

            for (auto& [name, sk] : settings.keybindsMap) {
                if (saved_kb.contains(name) && saved_kb[name].is_array()) {
                    sk.keys.clear();
                    sk.keys.reserve(saved_kb[name].size());
                    
                    for (const auto& k : saved_kb[name]) {
                        if (k.is_number_integer()) sk.keys.push_back(static_cast<u16>(k.get<int>()));
                    }
                }
            }
        }

        // Load Styles
        settings.styles.loadStyles(j);

        // Load graphics
        settings.vsyncEnabled = j.value("vsync", settings.vsyncEnabled);
    } catch (...) {
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

    json keybinds = json::object();
    for (const auto& [name, bind] : settings.keybindsMap) {
        keybinds[name] = bind.keys;
    }
    j["keybinds"] = keybinds;

    settings.styles.saveStyles(j);

    j["vsync"] = settings.vsyncEnabled;

    std::ofstream out(settings.settingsPath);
    out << j.dump(4);
}
