#include "AppSettings.hpp"

#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>

using json = nlohmann::json;

int AppSettings::version = 1;

bool AppSettings::load(AppContext& ctx) {
    if (!std::filesystem::exists(ctx.settingsPath)) {
        return false;
    }

    std::ifstream in(ctx.settingsPath);
    if (!in.is_open()) {
        return false;
    }

    try {
        json j;
        in >> j;

        AppSettings::version = j.value("version", 1);
        ctx.width = j.value("windowWidth",  ctx.width);
        ctx.height = j.value("windowHeight", ctx.height);
        ctx.posX = j.value("windowPositionX", ctx.posX);
        ctx.posY = j.value("windowPositionY", ctx.posY);
    } catch (...) {
        // malformed JSON -> fall back to defaults
        return false;
    }

    return true;
}

void AppSettings::save(const AppContext& ctx) {
    std::filesystem::create_directories(ctx.settingsPath.parent_path());

    json j;
    j["version"] = version;
    j["windowWidth"] = ctx.width;
    j["windowHeight"] = ctx.height;
    j["windowPositionX"] = ctx.posX;
    j["windowPositionY"] = ctx.posY;

    std::ofstream out(ctx.settingsPath);
    out << j.dump(4);
}
