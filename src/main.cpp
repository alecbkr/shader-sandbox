#include "application/Application.hpp"
#include "persistence/Paths.hpp"

#define APPLICATION_TITLE "Prism"

int main(int argc, char** argv) {
    AppContext ctx = AppContext(APPLICATION_TITLE);

    ctx.userConfigDir = Paths::getUserConfigDir(APPLICATION_TITLE);
    ctx.settingsPath = ctx.userConfigDir / "settings.json";
    AppSettings::load(ctx);

    if (!Application::initialize(ctx)) return 1;
    Application::runLoop(ctx);

    AppSettings::save(ctx);
    return 0;
}