// tests/core/input/test_input_state.cpp
#include <catch2/catch_amalgamated.hpp>

#include <memory>
#include <string>
#include <vector>

#include "core/input/InputState.hpp"
#include "core/logging/Logger.hpp"
#include "core/logging/LogSink.hpp"
#include "platform/components/Keys.hpp"

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

// Helpers: find an int key/button code that maps to your enum via translateGlfw*()
static int glfwKeyFor(Key target) {
    for (int k = 0; k <= 512; ++k) {
        if (translateGlfwKey(k) == target) return k;
    }
    return -1;
}

static int glfwMouseButtonFor(MouseButton target) {
    for (int b = 0; b <= 32; ++b) {
        if (translateGlfwMouseButton(b) == target) return b;
    }
    return -1;
}

TEST_CASE("InputState: getters are safe before initialize (return 0)", "[input][state]") {
    InputState in;

    REQUIRE(in.getMouseDeltaX() == 0.0);
    REQUIRE(in.getMouseDeltaY() == 0.0);
    REQUIRE(in.getScrollX() == 0.0);
    REQUIRE(in.getScrollY() == 0.0);

    REQUIRE(in.pressedKeys.empty());
}

TEST_CASE("InputState: initialize succeeds once; initialize twice returns false and logs warning", "[input][state]") {
    Logger logger;
    REQUIRE(logger.initialize());

    InputState in;
    REQUIRE(in.initialize(&logger) == true);

    auto sink = attachTestSink(logger);
    REQUIRE(in.initialize(&logger) == false);

    REQUIRE_FALSE(sink->entries.empty());
    const auto& last = sink->entries.back();
    REQUIRE(last.level == LogLevel::WARNING);
    REQUIRE(last.src == "Input State Initialization");
    REQUIRE(last.msg.find("already initialized") != std::string::npos);
}

TEST_CASE("InputState: beginFrame clears pressed/released/pressedKeys and resets deltas + scroll; down persists", "[input][state]") {
    Logger logger;
    REQUIRE(logger.initialize());

    InputState in;
    REQUIRE(in.initialize(&logger));

    const int kA = glfwKeyFor(Key::A);
    REQUIRE(kA != -1);

    // Press A this frame
    in.onKey(kA, 1); // press

    REQUIRE(in.isDownKey(Key::A));
    REQUIRE(in.wasPressed(Key::A));
    REQUIRE_FALSE(in.wasReleased(Key::A));
    REQUIRE(in.pressedKeys.size() == 1);
    REQUIRE(in.pressedKeys[0] == Key::A);

    // Update cursor + scroll to create deltas in-frame
    in.onCursorPos(10.0, 5.0);
    in.onCursorPos(15.0, 2.0); // adds (5, -3) on second move; total should be (15, 2) from (0,0) if starting at 0
    REQUIRE(in.getMouseDeltaX() == Catch::Approx(15.0));
    REQUIRE(in.getMouseDeltaY() == Catch::Approx(2.0));

    in.onScroll(1.0, -2.0);
    in.onScroll(0.5, 0.5);
    REQUIRE(in.getScrollX() == Catch::Approx(1.5));
    REQUIRE(in.getScrollY() == Catch::Approx(-1.5));

    // beginFrame clears edge flags, pressedKeys, deltas, scroll
    // but key remains down until released.
    in.beginFrame();

    REQUIRE(in.isDownKey(Key::A));
    REQUIRE_FALSE(in.wasPressed(Key::A));
    REQUIRE_FALSE(in.wasReleased(Key::A));
    REQUIRE(in.pressedKeys.empty());

    REQUIRE(in.getMouseDeltaX() == 0.0);
    REQUIRE(in.getMouseDeltaY() == 0.0);
    REQUIRE(in.getScrollX() == 0.0);
    REQUIRE(in.getScrollY() == 0.0);

    // Release A this frame
    in.onKey(kA, 0);
    REQUIRE_FALSE(in.isDownKey(Key::A));
    REQUIRE(in.wasReleased(Key::A));

    // Next frame clears released
    in.beginFrame();
    REQUIRE_FALSE(in.wasReleased(Key::A));
}

TEST_CASE("InputState: pressing an already-down key does not re-add to pressedKeys", "[input][state]") {
    Logger logger;
    REQUIRE(logger.initialize());

    InputState in;
    REQUIRE(in.initialize(&logger));

    const int kA = glfwKeyFor(Key::A);
    REQUIRE(kA != -1);

    in.onKey(kA, 1); // press
    REQUIRE(in.wasPressed(Key::A));
    REQUIRE(in.pressedKeys.size() == 1);

    // Another press while already down should not count as a new press
    in.onKey(kA, 1);
    REQUIRE(in.pressedKeys.size() == 1);

    // New frame: pressedKeys cleared
    in.beginFrame();
    REQUIRE(in.pressedKeys.empty());

    // Repeat action keeps it down but doesn't mark pressed
    in.onKey(kA, 2); // repeat
    REQUIRE(in.isDownKey(Key::A));
    REQUIRE_FALSE(in.wasPressed(Key::A));
}

TEST_CASE("InputState: release sets released flag only if key was down", "[input][state]") {
    Logger logger;
    REQUIRE(logger.initialize());

    InputState in;
    REQUIRE(in.initialize(&logger));

    const int kA = glfwKeyFor(Key::A);
    REQUIRE(kA != -1);

    // Release without prior down should not mark released
    in.onKey(kA, 0);
    REQUIRE_FALSE(in.wasReleased(Key::A));

    // Press then release should mark released
    in.onKey(kA, 1);
    in.beginFrame();       // clear pressed for clean check
    in.onKey(kA, 0);
    REQUIRE(in.wasReleased(Key::A));
}

TEST_CASE("InputState: mouse button press/release sets down/pressed/released correctly", "[input][state][mouse]") {
    Logger logger;
    REQUIRE(logger.initialize());

    InputState in;
    REQUIRE(in.initialize(&logger));

    const int bLeft = glfwMouseButtonFor(MouseButton::Left);
    REQUIRE(bLeft != -1);

    in.onMouseButton(bLeft, 1); // press
    REQUIRE(in.isDownMouse(MouseButton::Left));
    REQUIRE(in.wasPressedMouse(MouseButton::Left));
    REQUIRE_FALSE(in.wasReleasedMouse(MouseButton::Left));

    in.beginFrame(); // clear pressed/released
    REQUIRE(in.isDownMouse(MouseButton::Left));
    REQUIRE_FALSE(in.wasPressedMouse(MouseButton::Left));

    in.onMouseButton(bLeft, 0); // release
    REQUIRE_FALSE(in.isDownMouse(MouseButton::Left));
    REQUIRE(in.wasReleasedMouse(MouseButton::Left));

    in.beginFrame();
    REQUIRE_FALSE(in.wasReleasedMouse(MouseButton::Left));
}

TEST_CASE("InputState: onCursorPos accumulates deltas within a frame", "[input][state][mouse]") {
    Logger logger;
    REQUIRE(logger.initialize());

    InputState in;
    REQUIRE(in.initialize(&logger));

    // start at (0,0) from constructor/initialize
    in.onCursorPos(10.0, 5.0);   // delta += (10, 5)
    REQUIRE(in.getMouseDeltaX() == Catch::Approx(10.0));
    REQUIRE(in.getMouseDeltaY() == Catch::Approx(5.0));

    in.onCursorPos(15.0, 2.0);   // delta += (5, -3) => (15, 2)
    REQUIRE(in.getMouseDeltaX() == Catch::Approx(15.0));
    REQUIRE(in.getMouseDeltaY() == Catch::Approx(2.0));

    in.beginFrame();
    REQUIRE(in.getMouseDeltaX() == 0.0);
    REQUIRE(in.getMouseDeltaY() == 0.0);
}

TEST_CASE("InputState: onScroll accumulates and beginFrame clears scroll", "[input][state][scroll]") {
    Logger logger;
    REQUIRE(logger.initialize());

    InputState in;
    REQUIRE(in.initialize(&logger));

    in.onScroll(1.0, -2.0);
    REQUIRE(in.getScrollX() == Catch::Approx(1.0));
    REQUIRE(in.getScrollY() == Catch::Approx(-2.0));

    in.onScroll(0.5, 0.5);
    REQUIRE(in.getScrollX() == Catch::Approx(1.5));
    REQUIRE(in.getScrollY() == Catch::Approx(-1.5));

    in.beginFrame();
    REQUIRE(in.getScrollX() == 0.0);
    REQUIRE(in.getScrollY() == 0.0);
}

TEST_CASE("InputState: unknown key/button queries always return false", "[input][state]") {
    Logger logger;
    REQUIRE(logger.initialize());

    InputState in;
    REQUIRE(in.initialize(&logger));

    REQUIRE_FALSE(in.isDownKey(Key::Unknown));
    REQUIRE_FALSE(in.wasPressed(Key::Unknown));
    REQUIRE_FALSE(in.wasReleased(Key::Unknown));

    REQUIRE_FALSE(in.isDownMouse(MouseButton::Unknown));
    REQUIRE_FALSE(in.wasPressedMouse(MouseButton::Unknown));
    REQUIRE_FALSE(in.wasReleasedMouse(MouseButton::Unknown));
}
