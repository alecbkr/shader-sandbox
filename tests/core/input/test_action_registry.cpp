#include <catch2/catch_amalgamated.hpp>

#include <memory>
#include <string>
#include <vector>

#include "core/input/ActionRegistry.hpp"
#include "core/logging/Logger.hpp"
#include "core/logging/LogSink.hpp"

// Test sink captures LogEntry objects sent through Logger
struct TestSink final : public LogSink {
    std::vector<LogEntry> entries;
    void addLog(const LogEntry& entry) override { entries.push_back(entry); }
};

static std::shared_ptr<TestSink> attachTestSink(Logger& logger) {
    auto sink = std::make_shared<TestSink>();
    logger.addSink(sink);
    return sink;
}

TEST_CASE("ActionRegistry: initialize succeeds once", "[action][registry]") {
    Logger logger;
    REQUIRE(logger.initialize());

    ActionRegistry reg;
    REQUIRE(reg.initialize(&logger) == true);
}

TEST_CASE("ActionRegistry: initialize twice returns false and logs warning", "[action][registry]") {
    Logger logger;
    REQUIRE(logger.initialize());

    ActionRegistry reg;
    REQUIRE(reg.initialize(&logger) == true);

    auto sink = attachTestSink(logger);
    REQUIRE(reg.initialize(&logger) == false);

    REQUIRE_FALSE(sink->entries.empty());
    const auto& last = sink->entries.back();
    REQUIRE(last.level == LogLevel::WARNING);
    REQUIRE(last.src == "Action Registry Initialization");
    REQUIRE(last.msg.find("already initialized") != std::string::npos);
}

TEST_CASE("ActionRegistry: Action::None logs warning when triggered", "[action][registry]") {
    Logger logger;
    REQUIRE(logger.initialize());

    ActionRegistry reg;
    REQUIRE(reg.initialize(&logger) == true);

    auto sink = attachTestSink(logger);
    reg.trigger(Action::None);

    REQUIRE_FALSE(sink->entries.empty());
    const auto& last = sink->entries.back();
    REQUIRE(last.level == LogLevel::WARNING);
    REQUIRE(last.src == "Action::None Callback");
    REQUIRE(last.msg.find("placeholder action 'None'") != std::string::npos);
}

TEST_CASE("ActionRegistry: bind + trigger executes callback", "[action][registry]") {
    Logger logger;
    REQUIRE(logger.initialize());

    ActionRegistry reg;
    REQUIRE(reg.initialize(&logger) == true);

    int calls = 0;
    reg.bind(Action::SaveProject, [&] { calls++; });

    reg.trigger(Action::SaveProject);
    reg.trigger(Action::SaveProject);

    REQUIRE(calls == 2);
}

TEST_CASE("ActionRegistry: trigger on unbound action is a safe no-op", "[action][registry]") {
    Logger logger;
    REQUIRE(logger.initialize());

    ActionRegistry reg;
    REQUIRE(reg.initialize(&logger) == true);

    reg.trigger(Action::CameraUp); // never bound
    SUCCEED();
}

TEST_CASE("ActionRegistry: processActionsForFrame triggers queued actions in order then clears queue", "[action][registry]") {
    Logger logger;
    REQUIRE(logger.initialize());

    ActionRegistry reg;
    REQUIRE(reg.initialize(&logger) == true);

    std::vector<Action> seen;

    reg.bind(Action::CameraForward, [&] { seen.push_back(Action::CameraForward); });
    reg.bind(Action::CameraBack,    [&] { seen.push_back(Action::CameraBack); });
    reg.bind(Action::QuitApplication,[&] { seen.push_back(Action::QuitApplication); });

    reg.addActionToProcess(Action::CameraForward);
    reg.addActionToProcess(Action::CameraBack);
    reg.addActionToProcess(Action::QuitApplication);

    reg.processActionsForFrame();

    REQUIRE(seen == std::vector<Action>{
        Action::CameraForward,
        Action::CameraBack,
        Action::QuitApplication
    });

    // queue cleared
    reg.processActionsForFrame();
    REQUIRE(seen.size() == 3);
}

TEST_CASE("ActionRegistry: queued unbound actions are ignored during processing", "[action][registry]") {
    Logger logger;
    REQUIRE(logger.initialize());

    ActionRegistry reg;
    REQUIRE(reg.initialize(&logger) == true);

    int calls = 0;
    reg.bind(Action::CameraLeft, [&] { calls++; });

    reg.addActionToProcess(Action::CameraLeft);   // bound
    reg.addActionToProcess(Action::CameraRight);  // unbound -> should do nothing
    reg.addActionToProcess(Action::CameraLeft);   // bound again

    reg.processActionsForFrame();

    REQUIRE(calls == 2);
}
