// tests/core/input/test_context_manager.cpp
#include <catch2/catch_amalgamated.hpp>

#include <memory>
#include <string>
#include <vector>

#include "core/input/ContextManager.hpp"
#include "core/input/ActionRegistry.hpp"
#include "core/logging/Logger.hpp"
#include "core/logging/LogSink.hpp"

// ------------------------------------------------------------
// Test sink: captures logs from Logger
// ------------------------------------------------------------
struct TestSink final : public LogSink {
    std::vector<LogEntry> entries;
    void addLog(const LogEntry& entry) override { entries.push_back(entry); }
};

static std::shared_ptr<TestSink> attachTestSink(Logger& logger) {
    auto sink = std::make_shared<TestSink>();
    logger.addSink(sink);
    return sink;
}

TEST_CASE("ContextManager: default state is None and stack is empty (behaviorally)", "[context][manager]") {
    ContextManager ctx;

    REQUIRE(ctx.current() == ControlCtx::None);
    REQUIRE(ctx.is(ControlCtx::None));
    REQUIRE_FALSE(ctx.isEditor());
    REQUIRE_FALSE(ctx.isCamera());

    // pop on empty should be safe no-op
    ctx.pop();
    REQUIRE(ctx.current() == ControlCtx::None);
}

TEST_CASE("ContextManager: initialize sets current to Editor and clears stack", "[context][manager]") {
    Logger logger;
    REQUIRE(logger.initialize());

    ActionRegistry actions;
    REQUIRE(actions.initialize(&logger));

    ContextManager ctx;
    REQUIRE(ctx.initialize(&logger, &actions) == true);

    REQUIRE(ctx.current() == ControlCtx::Editor);
    REQUIRE(ctx.isEditor());
    REQUIRE_FALSE(ctx.isCamera());

    // Behaviorally verify stack was cleared: pop should do nothing
    ctx.pop();
    REQUIRE(ctx.current() == ControlCtx::Editor);
}

TEST_CASE("ContextManager: initialize twice returns false and logs warning", "[context][manager]") {
    Logger logger;
    REQUIRE(logger.initialize());

    ActionRegistry actions;
    REQUIRE(actions.initialize(&logger));

    ContextManager ctx;
    REQUIRE(ctx.initialize(&logger, &actions) == true);

    auto sink = attachTestSink(logger);

    REQUIRE(ctx.initialize(&logger, &actions) == false);

    REQUIRE_FALSE(sink->entries.empty());
    const auto& last = sink->entries.back();
    REQUIRE(last.level == LogLevel::WARNING);
    REQUIRE(last.src == "Context Manager Initialize");
    REQUIRE(last.msg.find("already initialized") != std::string::npos);
}

TEST_CASE("ContextManager: toggleCtx switches Editor <-> Camera using stack push", "[context][manager]") {
    Logger logger;
    REQUIRE(logger.initialize());

    ActionRegistry actions;
    REQUIRE(actions.initialize(&logger));

    ContextManager ctx;
    REQUIRE(ctx.initialize(&logger, &actions));

    REQUIRE(ctx.isEditor());

    ctx.toggleCtx();
    REQUIRE(ctx.isCamera());

    ctx.toggleCtx();
    REQUIRE(ctx.isEditor());
}

TEST_CASE("ContextManager: action binding SwitchControlContext triggers toggleCtx", "[context][manager][action]") {
    Logger logger;
    REQUIRE(logger.initialize());

    ActionRegistry actions;
    REQUIRE(actions.initialize(&logger));

    ContextManager ctx;
    REQUIRE(ctx.initialize(&logger, &actions));

    REQUIRE(ctx.isEditor());

    actions.trigger(Action::SwitchControlContext);
    REQUIRE(ctx.isCamera());

    actions.trigger(Action::SwitchControlContext);
    REQUIRE(ctx.isEditor());
}

TEST_CASE("ContextManager: push/pop behave like a stack of previous contexts", "[context][manager]") {
    ContextManager ctx;

    // start at None
    REQUIRE(ctx.current() == ControlCtx::None);

    ctx.push(ControlCtx::Editor);   // stack: [None], current = Editor
    REQUIRE(ctx.current() == ControlCtx::Editor);

    ctx.push(ControlCtx::Camera);   // stack: [None, Editor], current = Camera
    REQUIRE(ctx.current() == ControlCtx::Camera);

    ctx.pop();                      // current = Editor
    REQUIRE(ctx.current() == ControlCtx::Editor);

    ctx.pop();                      // current = None
    REQUIRE(ctx.current() == ControlCtx::None);

    ctx.pop();                      // empty -> no-op
    REQUIRE(ctx.current() == ControlCtx::None);
}

TEST_CASE("ContextManager: clearStack empties history so pop becomes no-op", "[context][manager]") {
    ContextManager ctx;

    ctx.set(ControlCtx::Editor);
    ctx.push(ControlCtx::Camera);   // stack has Editor
    REQUIRE(ctx.current() == ControlCtx::Camera);

    ctx.clearStack();
    ctx.pop(); // should do nothing because stack cleared

    REQUIRE(ctx.current() == ControlCtx::Camera);
}

TEST_CASE("ContextManager: set/current/is helpers work", "[context][manager]") {
    ContextManager ctx;

    ctx.set(ControlCtx::EditorCamera);
    REQUIRE(ctx.current() == ControlCtx::EditorCamera);
    REQUIRE(ctx.is(ControlCtx::EditorCamera));
    REQUIRE_FALSE(ctx.isEditor());
    REQUIRE_FALSE(ctx.isCamera());

    ctx.set(ControlCtx::Camera);
    REQUIRE(ctx.isCamera());
    REQUIRE_FALSE(ctx.isEditor());
}
