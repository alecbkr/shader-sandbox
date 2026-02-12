#include "application/Application.hpp"
#include "persistence/Paths.hpp"
#include "persistence/SettingsLoader.hpp"
#include "persistence/ProjectLoader.hpp"

#define APPLICATION_TITLE "PrismTSS"

int main(int argc, char** argv) {
    AppContext ctx = AppContext(APPLICATION_TITLE);

    ctx.project.projectRoot = Paths::getProjectRootDir(argc, argv, ctx.project.projectTitle);
    ctx.project.projectShadersDir = ctx.project.projectRoot / "shaders";
    ctx.project.projectJSON = ctx.project.projectRoot / "project.json";
    ProjectLoader::load(ctx.project);

    ctx.settings.userConfigDir = Paths::getUserConfigDir(APPLICATION_TITLE);
    ctx.settings.settingsPath = ctx.settings.userConfigDir / "settings.json";
    SettingsLoader::load(ctx.settings);

    if (!Application::initialize(ctx)) return 1;
    Application::runLoop(ctx);

    ProjectLoader::save(ctx.project);
    SettingsLoader::save(ctx.settings);
    return 0;
}