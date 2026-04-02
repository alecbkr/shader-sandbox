#include <catch2/catch_amalgamated.hpp>
#include <filesystem>
#include <fstream>

#include "application/SettingsStyles.hpp"
#include "core/logging/Logger.hpp"
#include "core/EventDispatcher.hpp"
#include "object/ModelCache.hpp"
#include "core/EditorEngine.hpp"
#include "core/ShaderRegistry.hpp"

// helper to take on the project's name for the logger 
static bool initTestLogger(Logger& logger){
    std::string testAppName = "PrimsTSS_Test"; 
    std::string testProjectName = "ActionRegistry_Tests"; 
    return logger.initialize(testAppName, testProjectName); 
}

// Helper to create a dummy shader file for testing
void CreateDummyFile(const std::string& path, const std::string& content) {
    std::ofstream ofs(path);
    ofs << content;
    ofs.close();
}

TEST_CASE("EditorEngine: Lifecycle and Initialization", "[editor_engine]") {
    Logger logger;
    EventDispatcher events;
    ModelCache cache;
    ShaderRegistry registry;
    SettingsStyles styles;
    Project project;

    initTestLogger(logger);
    events.initialize(&logger);

    EditorEngine engine;

    SECTION("Initialization attaches event listeners") {
        REQUIRE(engine.initialize(&logger, &events, &cache, &registry, &styles, &project) == true);

        // Verify double initialization fails
        REQUIRE(engine.initialize(&logger, &events, &cache, &registry, &styles, &project) == false);
    }

    SECTION("Shutdown resets state") {
        engine.initialize(&logger, &events, &cache, &registry, &styles, &project);
        engine.shutdown();

        // After shutdown, engine should be ready for a fresh init
        REQUIRE(engine.initialize(&logger, &events, &cache, &registry, &styles, &project) == true);
    }
}

TEST_CASE("EditorEngine: Editor Management via Events", "[editor_engine][events]") {
    Logger logger;
    EventDispatcher events;
    ModelCache cache;
    ShaderRegistry registry;
    SettingsStyles styles;
    Project project;

    initTestLogger(logger);
    events.initialize(&logger);

    EditorEngine engine;
    engine.initialize(&logger, &events, &cache, &registry, &styles, &project);

    // Setup a temp file
    std::string testPath = "test_shader.frag";
    std::string testContent = "void main() { gl_FragColor = vec4(1.0); }";
    CreateDummyFile(testPath, testContent);

    SECTION("Spawning an editor via OpenFile event") {
        events.TriggerEvent(OpenFileEvent(testPath, "test_shader.frag", false));
        events.ProcessQueue();

        REQUIRE(engine.editors.size() == 1);
        REQUIRE(engine.editors[0]->fileName == "test_shader.frag");
        // Verify TextEditor actually loaded the content
        REQUIRE(engine.editors[0]->textEditor.GetText() == testContent.append("\n"));
    }

    SECTION("Renaming an editor") {
        // Trigger Rename
        events.TriggerEvent(RenameFileEvent("test_shader.frag", "new_name.frag"));
        events.ProcessQueue();

        REQUIRE(engine.editors[0]->fileName == "new_name.frag");
        // Check if internal path updated extension/name correctly
        REQUIRE(std::filesystem::path(engine.editors[0]->filePath).filename() == "new_name.frag");
    }

    SECTION("Deleting an editor") {
        events.TriggerEvent(OpenFileEvent(testPath, "to_delete.frag", false));
        events.ProcessQueue();
        REQUIRE(engine.editors.size() == 1);

        events.TriggerEvent(DeleteFileEvent("to_delete.frag"));
        events.ProcessQueue();

        REQUIRE(engine.editors.empty());
    }

    // Cleanup
    std::filesystem::remove(testPath);
}

TEST_CASE("EditorEngine: Untitled File Generation", "[editor_engine][filesystem]") {
    Logger logger;
    EventDispatcher events;
    ModelCache cache;
    ShaderRegistry registry;
    SettingsStyles styles;
    Project project;

    initTestLogger(logger);
    events.initialize(&logger);

    EditorEngine engine;
    engine.initialize(&logger, &events, &cache, &registry, &styles, &project);

    // Ensure directory exists for the test
    std::filesystem::create_directories("../shaders");

    SECTION("Creating a new 'Untitled' file") {
        // Triggering NewFile (std::monostate)
        events.TriggerEvent(NewFileEvent());
        events.ProcessQueue();

        REQUIRE(engine.editors.size() == 1);
        std::string name = engine.editors[0]->fileName;
        REQUIRE(name.find("Untitled") != std::string::npos);

        // Verify the file was actually written to disk with the template
        REQUIRE(std::filesystem::exists(engine.editors[0]->filePath));
        std::string content = engine.editors[0]->textEditor.GetText();
        REQUIRE(content.find("#version 330 core") != std::string::npos);

        // Cleanup the created file
        std::filesystem::remove(engine.editors[0]->filePath);
    }
}