#include "application/Application.hpp"
#include "persistence/Paths.hpp"

#define APPLICATION_TITLE "Prism"

int main(int argc, char** argv) {
    AppContext ctx = AppContext(APPLICATION_TITLE);

    ctx.settings.userConfigDir = Paths::getUserConfigDir(APPLICATION_TITLE);
    ctx.settings.settingsPath = ctx.settings.userConfigDir / "settings.json";
    SettingsLoader::load(ctx.settings);

    if (!Application::initialize(ctx)) return 1;
    Application::runLoop(ctx);

    SettingsLoader::save(ctx.settings);
    return 0;
}