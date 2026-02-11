// tests/platform/test_platform.cpp
#include <catch2/catch_amalgamated.hpp>

#include <filesystem>

#include "platform/Platform.hpp"
#include "core/logging/Logger.hpp"
#include "core/input/ContextManager.hpp"
#include "core/input/Keybinds.hpp"
#include "core/input/ActionRegistry.hpp"
#include "core/input/InputState.hpp"
#include "application/AppContext.hpp"

TEST_CASE("Platform: methods are safe before initialize (guards)", "[platform]") {
    Platform p;

    // Should be safe no-ops / safe returns
    REQUIRE(p.shouldClose() == true);

    // These should not crash
    p.swapBuffers();
    p.pollEvents();
    p.processInput();

    // getExeDir() does not require initialization
    const auto exeDir = p.getExeDir();
    REQUIRE_FALSE(exeDir.empty());
}

TEST_CASE("Platform: getExeDir returns a valid-ish directory", "[platform]") {
    Platform p;

    const auto exeDir = p.getExeDir();

    // It should exist in normal runs. If it doesn't, at least it shouldn't be empty.
    // (On some weird environments current_path fallback could exist anyway.)
    REQUIRE_FALSE(exeDir.empty());

    // If it exists, it should be a directory.
    if (std::filesystem::exists(exeDir)) {
        REQUIRE(std::filesystem::is_directory(exeDir));
    }
}

TEST_CASE("Platform: initialize window + callbacks (integration, may skip)", "[platform][integration]") {
    Logger logger;
    REQUIRE(logger.initialize());

    ActionRegistry actions;
    REQUIRE(actions.initialize(&logger));

    ContextManager ctx;
    REQUIRE(ctx.initialize(&logger, &actions));
    
    InputState input;
    REQUIRE(input.initialize(&logger));

    Keybinds keybinds;
    REQUIRE(keybinds.initialize(&logger, &ctx, &actions, &input));

    AppContext app("test");
    Platform p;

    // Try to init. If this environment can't create a window / GL context,
    // don't fail the whole suite — skip this test.
    const bool ok = p.initialize(&logger, &ctx, &keybinds, &actions, &input, "sandbox_tests", &app);
    if (!ok) {
        SKIP("Platform initialize failed (likely headless/graphics unavailable in this environment).");
    }

    // If init succeeded, basic calls should work.
    REQUIRE(p.shouldClose() == false);

    // Polling events should be safe
    p.pollEvents();

    // getTime should return >= 0
    REQUIRE(p.getTime() >= 0.0);
}
