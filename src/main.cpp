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
    if (!ProjectLoader::load(ctx.project)) {
        std::cerr << "Failed to load project" << std::endl;
        return 1;
    }

    ctx.settings.userConfigDir = Paths::getUserConfigDir(APPLICATION_TITLE);
    ctx.settings.settingsPath = ctx.settings.userConfigDir / "settings.json";
    if (!SettingsLoader::load(ctx.settings)) {
        std::cerr << "Failed to load settings" << std::endl;
        return 1;
    }

    if (!Application::initialize(ctx)) 
    {
        std::cerr << "Application failed to initialize" << std::endl;
        return 1;
    }
    Application::runLoop(ctx);

    ProjectLoader::save(ctx.project);
    SettingsLoader::save(ctx.settings);
    return 0;
}
