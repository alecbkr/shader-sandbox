#include <catch2/catch_amalgamated.hpp>

#include <memory>
#include <string>
#include <vector>

#include "core/input/Keybinds.hpp"
#include "core/input/InputState.hpp"
#include "core/input/ActionRegistry.hpp"
#include "core/input/ContextManager.hpp"
#include "core/logging/Logger.hpp"
#include "platform/components/Keys.hpp"

// Helper: find an int GLFW key code that maps to your Key enum
static int glfwKeyFor(Key target) {
    for (int k = 0; k <= 512; ++k) {
        if (translateGlfwKey(k) == target) return k;
    }
    return -1;
}

static void pressKey(InputState& in, Key k) {
    const int code = glfwKeyFor(k);
    REQUIRE(code != -1);
    in.onKey(code, 1); // GLFW_PRESS
}

static void releaseKey(InputState& in, Key k) {
    const int code = glfwKeyFor(k);
    REQUIRE(code != -1);
    in.onKey(code, 0); // GLFW_RELEASE
}

TEST_CASE("KeyCombo normalize sorts by keyIndex and removes duplicates", "[keybinds][combo]") {
    KeyCombo combo{ Key::S, Key::LeftCtrl, Key::S, Key::A, Key::LeftCtrl };

    REQUIRE(combo.keys.size() == 3);

    for (size_t i = 1; i < combo.keys.size(); i++) {
        REQUIRE(keyIndex(combo.keys[i - 1]) < keyIndex(combo.keys[i]));
    }
}

TEST_CASE("Keybinds initialize creates expected default bindings", "[keybinds]") {
    Logger logger;
    REQUIRE(logger.initialize());

    ActionRegistry actions;
    REQUIRE(actions.initialize(&logger));

    ContextManager ctx;
    REQUIRE(ctx.initialize(&logger, &actions));

    Keybinds kb;
    REQUIRE(kb.initialize(&logger, &ctx, &actions));

    REQUIRE(kb.bindings().size() == 11);

    bool foundQuit = false;
    bool foundSave = false;
    bool foundSwitch = false;

    for (const Binding& b : kb.bindings()) {
        if (b.action == Action::QuitApplication) {
            foundQuit = true;
            REQUIRE(b.context == ControlCtx::Editor);
            REQUIRE(b.trigger == Trigger::Pressed);
            REQUIRE(b.combo.keys.size() == 2);
        }
        if (b.action == Action::SaveActiveShaderFile) {
            foundSave = true;
            REQUIRE(b.context == ControlCtx::Editor);
            REQUIRE(b.trigger == Trigger::Pressed);
            REQUIRE(b.combo.keys.size() == 2);
        }
        if (b.action == Action::SwitchControlContext) {
            foundSwitch = true;
            REQUIRE(b.context == ControlCtx::EditorCamera);
            REQUIRE(b.trigger == Trigger::Pressed);
            REQUIRE(b.combo.keys.size() == 1);
        }
    }

    REQUIRE(foundQuit);
    REQUIRE(foundSave);
    REQUIRE(foundSwitch);
}

TEST_CASE("Keybinds comboDown returns true only when all keys in combo are down", "[keybinds]") {
    Logger logger;
    REQUIRE(logger.initialize());

    ActionRegistry actions;
    REQUIRE(actions.initialize(&logger));

    ContextManager ctx;
    REQUIRE(ctx.initialize(&logger, &actions));

    InputState input;
    REQUIRE(input.initialize(&logger));

    Keybinds kb;
    REQUIRE(kb.initialize(&logger, &ctx, &actions));
    kb.setInputsPtr(&input);

    input.beginFrame();
    REQUIRE_FALSE(kb.comboDown(KeyCombo{Key::LeftCtrl, Key::S}));

    pressKey(input, Key::LeftCtrl);
    REQUIRE_FALSE(kb.comboDown(KeyCombo{Key::LeftCtrl, Key::S}));

    pressKey(input, Key::S);
    REQUIRE(kb.comboDown(KeyCombo{Key::LeftCtrl, Key::S}));

    releaseKey(input, Key::S);
    REQUIRE_FALSE(kb.comboDown(KeyCombo{Key::LeftCtrl, Key::S}));
}

TEST_CASE("Keybinds comboPressedThisFrame is true if combo is down AND at least one key was pressed this frame", "[keybinds]") {
    Logger logger;
    REQUIRE(logger.initialize());

    ActionRegistry actions;
    REQUIRE(actions.initialize(&logger));

    ContextManager ctx;
    REQUIRE(ctx.initialize(&logger, &actions));

    InputState input;
    REQUIRE(input.initialize(&logger));

    Keybinds kb;
    REQUIRE(kb.initialize(&logger, &ctx, &actions));
    kb.setInputsPtr(&input);

    // Frame 1: press Ctrl then S => should count as pressed-this-frame
    input.beginFrame();
    pressKey(input, Key::LeftCtrl);
    pressKey(input, Key::S);

    REQUIRE(kb.comboDown(KeyCombo{Key::LeftCtrl, Key::S}));
    REQUIRE(kb.comboPressedThisFrame(KeyCombo{Key::LeftCtrl, Key::S}));

    // Frame 2: hold both down but no new press => pressedThisFrame should be false
    input.beginFrame();
    REQUIRE(kb.comboDown(KeyCombo{Key::LeftCtrl, Key::S}));
    REQUIRE_FALSE(kb.comboPressedThisFrame(KeyCombo{Key::LeftCtrl, Key::S}));
}

TEST_CASE("Keybinds gatherActionsForFrame queues Pressed bindings in the correct context", "[keybinds]") {
    Logger logger;
    REQUIRE(logger.initialize());

    ActionRegistry actions;
    REQUIRE(actions.initialize(&logger));

    ContextManager ctx;
    REQUIRE(ctx.initialize(&logger, &actions));

    InputState input;
    REQUIRE(input.initialize(&logger));

    Keybinds kb;
    REQUIRE(kb.initialize(&logger, &ctx, &actions));
    kb.setInputsPtr(&input);

    int saveCalls = 0;
    actions.bind(Action::SaveActiveShaderFile, [&] { saveCalls++; });

    // Frame: press Ctrl+S in Editor
    input.beginFrame();
    pressKey(input, Key::LeftCtrl);
    pressKey(input, Key::S);

    kb.gatherActionsForFrame(ControlCtx::Editor);
    actions.processActionsForFrame();

    REQUIRE(saveCalls == 1);

    // Next frame: still holding keys but no new press => should NOT queue "Pressed" again
    input.beginFrame();
    kb.gatherActionsForFrame(ControlCtx::Editor);
    actions.processActionsForFrame();

    REQUIRE(saveCalls == 1);
}

TEST_CASE("Keybinds gatherActionsForFrame queues Down bindings every frame when held (Camera W)", "[keybinds]") {
    Logger logger;
    REQUIRE(logger.initialize());

    ActionRegistry actions;
    REQUIRE(actions.initialize(&logger));

    ContextManager ctx;
    REQUIRE(ctx.initialize(&logger, &actions));

    InputState input;
    REQUIRE(input.initialize(&logger));

    Keybinds kb;
    REQUIRE(kb.initialize(&logger, &ctx, &actions));
    kb.setInputsPtr(&input);

    int forwardCalls = 0;
    actions.bind(Action::CameraForward, [&] { forwardCalls++; });

    // Hold W down across two frames in Camera context
    input.beginFrame();
    pressKey(input, Key::W);

    kb.gatherActionsForFrame(ControlCtx::Camera);
    actions.processActionsForFrame();
    REQUIRE(forwardCalls == 1);

    input.beginFrame(); // pressed flags cleared, but down remains true
    kb.gatherActionsForFrame(ControlCtx::Camera);
    actions.processActionsForFrame();
    REQUIRE(forwardCalls == 2);
}

TEST_CASE("Keybinds EditorCamera bindings work regardless of context (F2)", "[keybinds]") {
    Logger logger;
    REQUIRE(logger.initialize());

    ActionRegistry actions;
    REQUIRE(actions.initialize(&logger));

    ContextManager ctx;
    REQUIRE(ctx.initialize(&logger, &actions));

    InputState input;
    REQUIRE(input.initialize(&logger));

    Keybinds kb;
    REQUIRE(kb.initialize(&logger, &ctx, &actions));
    kb.setInputsPtr(&input);

    int switchCalls = 0;
    actions.bind(Action::SwitchControlContext, [&] { switchCalls++; });

    // ---- Frame 1: press F2 in Editor ----
    input.beginFrame();
    pressKey(input, Key::F2);

    kb.gatherActionsForFrame(ControlCtx::Editor);
    actions.processActionsForFrame();
    REQUIRE(switchCalls == 1);

    // Release so we can press again next frame
    releaseKey(input, Key::F2);

    // ---- Frame 2: press F2 in Camera ----
    input.beginFrame();
    pressKey(input, Key::F2);

    kb.gatherActionsForFrame(ControlCtx::Camera);
    actions.processActionsForFrame();
    REQUIRE(switchCalls == 2);
}

TEST_CASE("Keybinds disabled binding does not queue action", "[keybinds]") {
    Logger logger;
    REQUIRE(logger.initialize());

    ActionRegistry actions;
    REQUIRE(actions.initialize(&logger));

    ContextManager ctx;
    REQUIRE(ctx.initialize(&logger, &actions));

    InputState input;
    REQUIRE(input.initialize(&logger));

    Keybinds kb;
    REQUIRE(kb.initialize(&logger, &ctx, &actions));
    kb.setInputsPtr(&input);

    int calls = 0;
    actions.bind(Action::None, [&] { calls++; });

    // Override bindings with one disabled binding on G in Editor
    kb.setBindings({
        kb.makeBinding(Action::None, KeyCombo{Key::G}, ControlCtx::Editor, Trigger::Pressed, false)
    });

    input.beginFrame();
    pressKey(input, Key::G);

    kb.gatherActionsForFrame(ControlCtx::Editor);
    actions.processActionsForFrame();

    REQUIRE(calls == 0);
}
