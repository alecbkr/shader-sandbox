#include <catch2/catch_amalgamated.hpp>

#include <memory>
#include <string>
#include <vector>

#include "core/EventDispatcher.hpp"
#include "core/logging/Logger.hpp"
#include "core/logging/LogSink.hpp"

// ------------------------------------------------------------
// Test sink: captures logs from Logger (for double-initialize warning)
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

TEST_CASE("EventDispatcher: does nothing when not initialized", "[event][dispatcher]") {
    EventDispatcher d;

    int calls = 0;

    d.Subscribe(EventType::SaveActiveShaderFile, [&](const EventPayload&) {
        calls++;
        return true;
    });

    d.TriggerEvent(MakeSaveActiveShaderFileEvent("abc.glsl", 7));
    d.ProcessQueue();

    REQUIRE(calls == 0); // not initialized => no-ops
}

TEST_CASE("EventDispatcher: initialize succeeds once; initialize twice returns false and logs warning", "[event][dispatcher]") {
    Logger logger;
    REQUIRE(logger.initialize());

    EventDispatcher d;
    REQUIRE(d.initialize(&logger) == true);

    auto sink = attachTestSink(logger);
    REQUIRE(d.initialize(&logger) == false);

    REQUIRE_FALSE(sink->entries.empty());
    const auto& last = sink->entries.back();
    REQUIRE(last.level == LogLevel::WARNING);
    REQUIRE(last.src == "Event Dispatcher Initialization");
    REQUIRE(last.msg.find("already initialized") != std::string::npos);
}

TEST_CASE("EventDispatcher: subscribed listener is called for matching event type", "[event][dispatcher]") {
    Logger logger;
    REQUIRE(logger.initialize());

    EventDispatcher d;
    REQUIRE(d.initialize(&logger));

    int calls = 0;
    d.Subscribe(EventType::Quit, [&](const EventPayload& p) {
        calls++;
        REQUIRE(std::holds_alternative<std::monostate>(p));
        return false;
    });

    d.TriggerEvent(MakeQuitAppEvent());
    d.ProcessQueue();

    REQUIRE(calls == 1);
}

TEST_CASE("EventDispatcher: listener is NOT called for different event type", "[event][dispatcher]") {
    Logger logger;
    REQUIRE(logger.initialize());

    EventDispatcher d;
    REQUIRE(d.initialize(&logger));

    int calls = 0;
    d.Subscribe(EventType::SaveActiveShaderFile, [&](const EventPayload&) {
        calls++;
        return false;
    });

    // Trigger a DIFFERENT type that exists in your header
    d.TriggerEvent(MakeQuitAppEvent());
    d.ProcessQueue();

    REQUIRE(calls == 0);
}

TEST_CASE("EventDispatcher: events are processed FIFO across types", "[event][dispatcher]") {
    Logger logger;
    REQUIRE(logger.initialize());

    EventDispatcher d;
    REQUIRE(d.initialize(&logger));

    std::vector<EventType> seen;

    d.Subscribe(EventType::Quit, [&](const EventPayload&) {
        seen.push_back(EventType::Quit);
        return false;
    });

    d.Subscribe(EventType::WindowResize, [&](const EventPayload&) {
        seen.push_back(EventType::WindowResize);
        return false;
    });

    d.TriggerEvent(MakeQuitAppEvent());
    d.TriggerEvent(MakeWindowResizeEvent(800, 600));
    d.TriggerEvent(MakeQuitAppEvent());
    d.ProcessQueue();

    REQUIRE(seen.size() == 3);
    REQUIRE(seen[0] == EventType::Quit);
    REQUIRE(seen[1] == EventType::WindowResize);
    REQUIRE(seen[2] == EventType::Quit);
}

TEST_CASE("EventDispatcher: handled stops later listeners for same event", "[event][dispatcher]") {
    Logger logger;
    REQUIRE(logger.initialize());

    EventDispatcher d;
    REQUIRE(d.initialize(&logger));

    int firstCalls = 0;
    int secondCalls = 0;

    d.Subscribe(EventType::ReloadShader, [&](const EventPayload& p) {
        firstCalls++;
        REQUIRE(std::holds_alternative<ReloadShaderPayload>(p));
        return true; // handled -> stop propagation
    });

    d.Subscribe(EventType::ReloadShader, [&](const EventPayload&) {
        secondCalls++;
        return false;
    });

    d.TriggerEvent(MakeReloadShaderEvent("MyProgram"));
    d.ProcessQueue();

    REQUIRE(firstCalls == 1);
    REQUIRE(secondCalls == 0);
}

TEST_CASE("EventDispatcher: if not handled, all listeners run", "[event][dispatcher]") {
    Logger logger;
    REQUIRE(logger.initialize());

    EventDispatcher d;
    REQUIRE(d.initialize(&logger));

    int a = 0, b = 0, c = 0;

    d.Subscribe(EventType::Quit, [&](const EventPayload&) { a++; return false; });
    d.Subscribe(EventType::Quit, [&](const EventPayload&) { b++; return false; });
    d.Subscribe(EventType::Quit, [&](const EventPayload&) { c++; return false; });

    d.TriggerEvent(MakeQuitAppEvent());
    d.ProcessQueue();

    REQUIRE(a == 1);
    REQUIRE(b == 1);
    REQUIRE(c == 1);
}

TEST_CASE("EventDispatcher: payload arrives intact (SaveActiveShaderFile)", "[event][dispatcher][payload]") {
    Logger logger;
    REQUIRE(logger.initialize());

    EventDispatcher d;
    REQUIRE(d.initialize(&logger));

    std::string gotPath;
    unsigned int gotModel = 0;
    int calls = 0;

    d.Subscribe(EventType::SaveActiveShaderFile, [&](const EventPayload& p) {
        calls++;
        REQUIRE(std::holds_alternative<SaveActiveShaderFilePayload>(p));
        const auto& payload = std::get<SaveActiveShaderFilePayload>(p);
        gotPath = payload.filePath;
        gotModel = payload.modelID;
        return true;
    });

    d.TriggerEvent(MakeSaveActiveShaderFileEvent("shaders/test.glsl", 42));
    d.ProcessQueue();

    REQUIRE(calls == 1);
    REQUIRE(gotPath == "shaders/test.glsl");
    REQUIRE(gotModel == 42u);
}

TEST_CASE("EventDispatcher: payload arrives intact (OpenFile)", "[event][dispatcher][payload]") {
    Logger logger;
    REQUIRE(logger.initialize());

    EventDispatcher d;
    REQUIRE(d.initialize(&logger));

    OpenFilePayload got{};
    int calls = 0;

    d.Subscribe(EventType::OpenFile, [&](const EventPayload& p) {
        calls++;
        REQUIRE(std::holds_alternative<OpenFilePayload>(p));
        got = std::get<OpenFilePayload>(p);
        return true;
    });

    d.TriggerEvent(OpenFileEvent("C:/x/y", "file.glsl", 9));
    d.ProcessQueue();

    REQUIRE(calls == 1);
    REQUIRE(got.filePath == "C:/x/y");
    REQUIRE(got.fileName == "file.glsl");
    REQUIRE(got.modelID == 9u);
}

TEST_CASE("EventDispatcher: payload arrives intact (WindowResize)", "[event][dispatcher][payload]") {
    Logger logger;
    REQUIRE(logger.initialize());

    EventDispatcher d;
    REQUIRE(d.initialize(&logger));

    int gotW = 0, gotH = 0;
    int calls = 0;

    d.Subscribe(EventType::WindowResize, [&](const EventPayload& p) {
        calls++;
        REQUIRE(std::holds_alternative<WindowResizePayload>(p));
        const auto& payload = std::get<WindowResizePayload>(p);
        gotW = payload.w;
        gotH = payload.h;
        return true;
    });

    d.TriggerEvent(MakeWindowResizeEvent(1280, 720));
    d.ProcessQueue();

    REQUIRE(calls == 1);
    REQUIRE(gotW == 1280);
    REQUIRE(gotH == 720);
}

TEST_CASE("EventDispatcher: shutdown clears listeners + queue and disables processing", "[event][dispatcher]") {
    Logger logger;
    REQUIRE(logger.initialize());

    EventDispatcher d;
    REQUIRE(d.initialize(&logger));

    int calls = 0;
    d.Subscribe(EventType::Quit, [&](const EventPayload&) { calls++; return false; });

    // Queue an event, then shutdown before processing
    d.TriggerEvent(MakeQuitAppEvent());
    d.shutdown();

    // not initialized => no processing
    d.ProcessQueue();
    REQUIRE(calls == 0);

    // Re-init should start clean (no listeners)
    REQUIRE(d.initialize(&logger) == true);
    d.TriggerEvent(MakeQuitAppEvent());
    d.ProcessQueue();
    REQUIRE(calls == 0);
}
