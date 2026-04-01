#include "application/Application.hpp"

#include "persistence/Paths.hpp"
#include "persistence/SettingsLoader.hpp"
#include "persistence/ProjectLoader.hpp"


#define APPLICATION_TITLE "PrismTSS"

int main(int argc, char** argv) {
    AppContext ctx = AppContext(APPLICATION_TITLE);

    ctx.settings.userConfigDir = Paths::getUserConfigDir(APPLICATION_TITLE);
    ctx.settings.settingsPath = ctx.settings.userConfigDir / "settings.json";
    SettingsLoader::load(ctx.settings);

    ctx.project.projectRoot = Paths::getProjectRootDir(ctx.settings.projectToOpen, ctx.project.projectTitle);
    ctx.project.projectShadersDir = ctx.project.projectRoot / "shaders";
    ctx.project.projectAssetsDir = ctx.project.projectRoot / "assets";
    ctx.project.projectJSON = ctx.project.projectRoot / "project.json";
    ctx.project.shaderRegistry = &ctx.shader_registry;
    ctx.project.uniformRegistry = &ctx.uniform_registry;
    ctx.project.events = &ctx.events;
    bool assetsAreLoaded = ProjectLoader::loadAssets(ctx.project);

    ctx.settings.projectToOpen = ctx.project.projectTitle;
    SettingsLoader::save(ctx.settings);

    if (!Application::initialize(ctx))
    {
        std::cerr << "Application failed to initialize" << std::endl;
        return 1;
    }
    if (assetsAreLoaded) {
        ProjectLoader::load(ctx.project);
    }
    else {
        Application::loadDefaultScene(ctx);
    }

    Application::runLoop(ctx);
    Application::shutdown(ctx);

    if (!ctx.projectSwitch) ctx.settings.projectToOpen = ctx.project.projectTitle;
    ProjectLoader::save(ctx.project, &ctx.model_cache, &ctx.material_cache);
    SettingsLoader::save(ctx.settings);

    if (ctx.projectSwitch) main(argc, argv);

    return 0;
}
