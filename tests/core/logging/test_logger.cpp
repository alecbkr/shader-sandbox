// tests/core/test_logger.cpp
#include <catch2/catch_amalgamated.hpp>

#include <algorithm>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

// Your headers
#include "core/logging/Logger.hpp"
#include "core/logging/LogSink.hpp"   // adjust if LogSink is included via Logger.hpp
// (LogEntry + LogLevel must be visible from these includes)

// ------------------------------------------------------------
// Test sink: captures all logs sent to it
// ------------------------------------------------------------
struct TestSink final : public LogSink {
    std::vector<LogEntry> entries;

    void addLog(const LogEntry& entry) override {
        entries.push_back(entry);
    }
};

// ------------------------------------------------------------
// std::cout capture helper (for "not initialized" message)
// ------------------------------------------------------------
struct CoutCapture {
    std::streambuf* old = nullptr;
    std::ostringstream ss;

    CoutCapture() { old = std::cout.rdbuf(ss.rdbuf()); }
    ~CoutCapture() { std::cout.rdbuf(old); }

    std::string str() const { return ss.str(); }
};

TEST_CASE("Logger: getConsoleSinkPtr returns nullptr before initialize", "[logger]") {
    Logger logger;
    REQUIRE(logger.getConsoleSinkPtr() == nullptr);
}

TEST_CASE("Logger: addLog before initialize prints warning and does not dispatch", "[logger]") {
    Logger logger;

    auto sink = std::make_shared<TestSink>();
    logger.addSink(sink); // even if someone adds sinks early, addLog should bail

    CoutCapture cap;
    logger.addLog(LogLevel::INFO, "UnitTest", "Hello");

    REQUIRE(cap.str().find("Attempting to add log without initializing the logger!") != std::string::npos);
    REQUIRE(sink->entries.empty());
}

TEST_CASE("Logger: initialize returns true and console sink becomes available", "[logger]") {
    Logger logger;

    REQUIRE(logger.initialize() == true);
    REQUIRE(logger.getConsoleSinkPtr() != nullptr);
}

TEST_CASE("Logger: dispatches logs to sinks added after initialize", "[logger]") {
    Logger logger;
    REQUIRE(logger.initialize() == true);

    auto sink = std::make_shared<TestSink>();
    logger.addSink(sink);

    logger.addLog(LogLevel::WARNING, "LoggerTest", "Message", "Extra", 123);

    REQUIRE(sink->entries.size() == 1);

    const auto& e = sink->entries[0];
    REQUIRE(e.level == LogLevel::WARNING);
    REQUIRE(e.src == "LoggerTest");
    REQUIRE(e.msg == "Message");
    REQUIRE(e.additional == "Extra");
    REQUIRE(e.lineNum == 123);
}

TEST_CASE("Logger: removeSink stops dispatching to that sink", "[logger]") {
    Logger logger;
    REQUIRE(logger.initialize() == true);

    auto sink = std::make_shared<TestSink>();
    logger.addSink(sink);

    logger.addLog(LogLevel::INFO, "A", "One");
    REQUIRE(sink->entries.size() == 1);

    logger.removeSink(sink);
    logger.addLog(LogLevel::INFO, "A", "Two");

    REQUIRE(sink->entries.size() == 1); // second log not delivered
}

TEST_CASE("Logger: CRITICAL does not exit; still dispatches like any other log", "[logger]") {
    Logger logger;
    REQUIRE(logger.initialize() == true);

    auto sink = std::make_shared<TestSink>();
    logger.addSink(sink);

    // Should NOT terminate now that exit() is removed
    logger.addLog(LogLevel::CRITICAL, "Core", "Something bad happened");

    REQUIRE(sink->entries.size() == 1);
    REQUIRE(sink->entries[0].level == LogLevel::CRITICAL);
    REQUIRE(sink->entries[0].src == "Core");
    REQUIRE(sink->entries[0].msg == "Something bad happened");
}
