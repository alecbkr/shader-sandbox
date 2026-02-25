#include <catch2/catch_amalgamated.hpp>

#include <functional>
#include <memory>
#include <vector>

#include "engine/AppTimer.hpp"
#include "core/logging/Logger.hpp"
#include "core/logging/LogSink.hpp"

// --- capture logs for double-initialize test ---
struct TestSink final : public LogSink {
    std::vector<LogEntry> entries;
    void addLog(const LogEntry& entry) override { entries.push_back(entry); }
};

static std::shared_ptr<TestSink> attachTestSink(Logger& logger) {
    auto sink = std::make_shared<TestSink>();
    logger.addSink(sink);
    return sink;
}

TEST_CASE("AppTimer: update before initialize is a no-op (dt stays 0)", "[apptimer]") {
    AppTimer t;

    t.update();
    REQUIRE(t.getDt() == 0.0f);

    // getFPS should remain 0 as well
    REQUIRE(t.getFPS() == 0.0f);
}

TEST_CASE("AppTimer: initialize twice returns false and logs warning", "[apptimer]") {
    Logger logger;
    REQUIRE(logger.initialize());

    double fakeNow = 0.0;
    AppTimer t;

    REQUIRE(t.initialize(&logger, [&] { return fakeNow; }) == true);

    auto sink = attachTestSink(logger);
    REQUIRE(t.initialize(&logger, [&] { return fakeNow; }) == false);

    REQUIRE_FALSE(sink->entries.empty());
    const auto& last = sink->entries.back();
    REQUIRE(last.level == LogLevel::WARNING);
    REQUIRE(last.src == "App Timer Initialize");
    REQUIRE(last.msg.find("already been initialized") != std::string::npos);
}

TEST_CASE("AppTimer: update computes dt from successive time reads", "[apptimer]") {
    Logger logger;
    REQUIRE(logger.initialize());

    double fakeNow = 0.0;
    AppTimer t;
    REQUIRE(t.initialize(&logger, [&] { return fakeNow; }));

    // First update: last=0, curr=0 -> dt = 0
    t.update();
    REQUIRE(t.getDt() == 0.0f);

    // Advance time and update
    fakeNow = 0.10;
    t.update();
    REQUIRE(t.getDt() == Catch::Approx(0.10f));

    fakeNow = 0.15;
    t.update();
    REQUIRE(t.getDt() == Catch::Approx(0.05f));
}

TEST_CASE("AppTimer: FPS updates when accumulated elapsed time >= 1 second", "[apptimer]") {
    Logger logger;
    REQUIRE(logger.initialize());

    // Simulate fixed timestep ~60 FPS
    double fakeNow = 0.0;
    const double dt = 1.0 / 60.0;

    AppTimer t;
    REQUIRE(t.initialize(&logger, [&] { return fakeNow; }));

    float fps = 0.0f;

    // Run 60 frames -> elapsed ~ 1.0 seconds, FPS should update near 60
    for (int i = 0; i < 60; i++) {
        t.update();          // sets deltaTime based on fakeNow increments
        fps = t.getFPS();    // accumulates elapsed and maybe updates fps
        fakeNow += dt;
    }

    // Depending on floating accumulation, this might update exactly at frame 60 or shortly after.
    // If it didn't update yet, run a couple more frames until it does.
    int safety = 0;
    while (fps == 0.0f && safety++ < 5) {
        t.update();
        fps = t.getFPS();
        fakeNow += dt;
    }

    REQUIRE(fps > 0.0f);
    REQUIRE(fps == Catch::Approx(60.0f).margin(1.0f));
}

TEST_CASE("AppTimer: FPS stays the same between updates if < 1 second elapsed", "[apptimer]") {
    Logger logger;
    REQUIRE(logger.initialize());

    double fakeNow = 0.0;
    AppTimer t;
    REQUIRE(t.initialize(&logger, [&] { return fakeNow; }));

    // Advance small deltas, shouldn't reach 1 second => fps remains 0
    for (int i = 0; i < 10; i++) {
        t.update();
        REQUIRE(t.getFPS() == 0.0f);
        fakeNow += 0.05; // total 0.5 seconds
    }
}
