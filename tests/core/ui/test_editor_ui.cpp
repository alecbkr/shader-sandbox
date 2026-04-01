#include <catch2/catch_amalgamated.hpp>

#include "application/SettingsStyles.hpp"
#include "core/EditorEngine.hpp"
#include "core/ui/EditorUI.hpp" // Assuming the header location
#include "core/logging/Logger.hpp"
#include "core/EventDispatcher.hpp"
#include "core/input/ContextManager.hpp"
#include "object/ModelCache.hpp"
#include "core/ShaderRegistry.hpp"

// helper to take on the project's name for the logger 
static bool initTestLogger(Logger& logger){
    std::string testAppName = "PrimsTSS_Test"; 
    std::string testProjectName = "ActionRegistry_Tests"; 
    return logger.initialize(testAppName, testProjectName); 
}

TEST_CASE("EditorUI: Initialization", "[editor_ui]") {
    Logger logger;
    EditorEngine engine;
    ContextManager context;
    SettingsStyles styles;
    ModelCache cache;
    ShaderRegistry registry;
    EventDispatcher events;
    Project project;
    Fonts fonts;

    initTestLogger(logger);
    engine.initialize(&logger, &events, &cache, &registry, &styles, &project);

    EditorUI ui;

    SECTION("Successful initialization") {
        REQUIRE(ui.initialize(&logger, &engine, &context, &events, &project, &fonts) == true);
    }

    SECTION("Prevent double initialization") {
        ui.initialize(&logger, &engine, &context, &events, &project, &fonts);
        // Second call should return false
        REQUIRE(ui.initialize(&logger, &engine, &context, &events, &project, &fonts) == false);
    }
}

TEST_CASE("EditorUI: Tab Management", "[editor_ui]") {
    Logger logger;
    EventDispatcher events;
    EditorEngine engine;
    ContextManager context;
    ModelCache cache;
    ShaderRegistry registry;
    SettingsStyles styles;
    Project project;
    Fonts fonts;

    initTestLogger(logger);
    events.initialize(&logger);
    engine.initialize(&logger, &events, &cache, &registry, &styles, &project);

    EditorUI ui;
    ui.initialize(&logger, &engine, &context, &events, &project, &fonts);

    SECTION("Closing a tab removes it from engine") {
        events.TriggerEvent(OpenFileEvent("file1.frag", "file1.frag", false));
        events.TriggerEvent(OpenFileEvent("file2.frag", "file2.frag", false));
        events.ProcessQueue();

        REQUIRE(engine.editors.size() == 2);

        // Simulation: In a real UI, the user clicks the 'x' on the tab.
        // We trigger the logic found inside your render() loop:
        engine.editors[0]->destroy();
        engine.editors.erase(engine.editors.begin() + 0);

        REQUIRE(engine.editors.size() == 1);
        REQUIRE(engine.editors[0]->fileName == "file2.frag");
    }
}