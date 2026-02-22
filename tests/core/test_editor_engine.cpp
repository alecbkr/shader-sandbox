#include <catch2/catch_amalgamated.hpp>
#include <filesystem>
#include <fstream>

#include "core/logging/Logger.hpp"
#include "core/EventDispatcher.hpp"
#include "object/ModelCache.hpp"
#include "core/EditorEngine.hpp"
#include "core/ShaderRegistry.hpp"

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

    logger.initialize();
    events.initialize(&logger);

    EditorEngine engine;

    SECTION("Initialization attaches event listeners") {
        REQUIRE(engine.initialize(&logger, &events, &cache, &registry) == true);

        // Verify double initialization fails
        REQUIRE(engine.initialize(&logger, &events, &cache, &registry) == false);
    }

    SECTION("Shutdown resets state") {
        engine.initialize(&logger, &events, &cache, &registry);
        engine.shutdown();

        // After shutdown, engine should be ready for a fresh init
        REQUIRE(engine.initialize(&logger, &events, &cache, &registry) == true);
    }
}

TEST_CASE("EditorEngine: Editor Management via Events", "[editor_engine][events]") {
    Logger logger;
    EventDispatcher events;
    ModelCache cache;
    ShaderRegistry registry;

    logger.initialize();
    events.initialize(&logger);

    EditorEngine engine;
    engine.initialize(&logger, &events, &cache, &registry);

    // Setup a temp file
    std::string testPath = "test_shader.frag";
    std::string testContent = "void main() { gl_FragColor = vec4(1.0); }";
    CreateDummyFile(testPath, testContent);

    SECTION("Spawning an editor via OpenFile event") {
        events.TriggerEvent(OpenFileEvent(testPath, "test_shader.frag", 0));
        events.ProcessQueue();

        REQUIRE(engine.editors.size() == 1);
        REQUIRE(engine.editors[0]->fileName == "test_shader.frag");
        // Verify TextEditor actually loaded the content
        REQUIRE(engine.editors[0]->textEditor.GetText() == testContent.append("\n"));
    }

    SECTION("Renaming an editor") {
        // First spawn one
        events.TriggerEvent(OpenFileEvent(testPath, "old_name.frag", 0));
        events.ProcessQueue();

        // Trigger Rename
        events.TriggerEvent(RenameFileEvent("old_name.frag", "new_name.frag"));
        events.ProcessQueue();

        REQUIRE(engine.editors[0]->fileName == "new_name.frag");
        // Check if internal path updated extension/name correctly
        REQUIRE(std::filesystem::path(engine.editors[0]->filePath).filename() == "new_name.frag");
    }

    SECTION("Deleting an editor") {
        events.TriggerEvent(OpenFileEvent(testPath, "to_delete.frag", 0));
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

    logger.initialize();
    events.initialize(&logger);

    EditorEngine engine;
    engine.initialize(&logger, &events, &cache, &registry);

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

TEST_CASE("EditorEngine: Model ID Linking", "[editor_engine]") {
    Logger logger;
    EventDispatcher events;
    ModelCache cache;
    ShaderRegistry registry;

    logger.initialize();
    events.initialize(&logger);

    EditorEngine engine;
    engine.initialize(&logger, &events, &cache, &registry);

    SECTION("Editor inherits ModelID from payload") {
        events.TriggerEvent(OpenFileEvent("path.glsl", "name.glsl", 1234u));
        events.ProcessQueue();

        REQUIRE(engine.editors.back()->modelID == 1234u);
    }
}