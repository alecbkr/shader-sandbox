// #define CATCH_CONFIG_MAIN
// #include <catch2/catch_amalgamated.hpp>
// #include <memory>
// #include <deque>
// #include <string>

// // Include your Engine Header
// #include "core/ConsoleEngine.hpp"

// // =========================================================
// // MOCKS / STUBS
// // =========================================================
// // We mock these dependencies to test ConsoleEngine in isolation.

// // 1. Mock ConsoleSink
// class ConsoleSink {
// public:
//     std::deque<LogEntry> mockLogs;

//     const std::deque<LogEntry>& getLogs() const {
//         return mockLogs;
//     }

//     void clearLogs() {
//         mockLogs.clear();
//     }

//     // Helper for tests to inject logs
//     void addMockLog(const LogEntry& entry) {
//         mockLogs.push_back(entry);
//     }
// };

// // 2. Mock Logger
// class Logger {
// public:
//     std::shared_ptr<ConsoleSink> sink;

//     Logger() {
//         sink = std::make_shared<ConsoleSink>();
//     }

//     std::shared_ptr<ConsoleSink> getConsoleSinkPtr() {
//         return sink;
//     }

//     void addLog(LogLevel level, std::string src, std::string msg, std::string additional = "") {
//         // No-op for engine tests, or simple print
//     }
    
//     // Mock path for history tests
//     std::filesystem::path getLogPath() {
//         return "test_log.txt";
//     }
// };

// // =========================================================
// // TESTS
// // =========================================================

// TEST_CASE("ConsoleEngine Initialization", "[ConsoleEngine]") {
//     Logger mockLogger;
//     ConsoleEngine engine;

//     SECTION("Initializes successfully with valid logger") {
//         bool result = engine.initialize(&mockLogger);
//         REQUIRE(result == true);
//     }

//     SECTION("Prevents double initialization") {
//         engine.initialize(&mockLogger);
//         bool result = engine.initialize(&mockLogger);
//         REQUIRE(result == false);
//     }
// }

// TEST_CASE("ConsoleEngine Toggles and Actions", "[ConsoleEngine]") {
//     Logger mockLogger;
//     ConsoleEngine engine;
//     engine.initialize(&mockLogger);

//     SECTION("Default Toggles are set correctly") {
//         const auto& toggles = engine.getToggles();
//         CHECK(toggles.isShowError == true);
//         CHECK(toggles.isShowInfo == true);
//         CHECK(toggles.isAutoScroll == true);
//     }

//     SECTION("Registering and Triggering Toggles updates state") {
//         // Simulate clicking "Show Errors" toggle off
//         engine.executeBtnAction("show_errors"); // Note: Ensure your engine maps this string or manually call the toggle logic if not mapped by default
        
//         // Since we didn't map the string explicitly in initialize() for the test (unless it's in your cpp),
//         // we can test the specific register/callback mechanism:
//         bool callbackState = false;
//         engine.registerToggle("test_toggle", [&](bool state) { callbackState = state; });
        
//         // Simulating UI interaction usually calls the lambda directly, 
//         // but let's test the state change via the engine's toggle struct directly if we can access it.
//         auto& toggles = engine.getToggles();
        
//         // Manually flip a toggle to verify struct access
//         toggles.isShowError = false;
//         CHECK(engine.getToggles().isShowError == false);
//     }
// }

// TEST_CASE("ConsoleEngine Filtering Logic", "[ConsoleEngine]") {
//     Logger mockLogger;
//     ConsoleEngine engine;
//     engine.initialize(&mockLogger);
//     auto& toggles = engine.getToggles();

//     // Prepare sample logs
//     LogEntry errorLog;
//     errorLog.level = LogLevel::LOG_ERROR;
//     errorLog.msg = "Error Message";
//     errorLog.category = LogCategory::SYSTEM;

//     LogEntry infoLog;
//     infoLog.level = LogLevel::INFO;
//     infoLog.msg = "Info Message";
//     infoLog.category = LogCategory::SYSTEM;
    
//     LogEntry uiLog;
//     uiLog.level = LogLevel::INFO;
//     uiLog.msg = "UI Message";
//     uiLog.category = LogCategory::UI;

//     LogEntry emptyLog;
//     emptyLog.msg = "   "; // Whitespace
//     emptyLog.level = LogLevel::INFO;

//     SECTION("Filters by Log Level") {
//         toggles.isShowError = true;
//         CHECK(engine.isLogFiltered(errorLog) == false);

//         toggles.isShowError = false;
//         CHECK(engine.isLogFiltered(errorLog) == true);
//     }

//     SECTION("Filters by Log Category") {
//         toggles.isShowUI = true;
//         CHECK(engine.isLogFiltered(uiLog) == false);

//         toggles.isShowUI = false;
//         CHECK(engine.isLogFiltered(uiLog) == true);
//     }

//     SECTION("Automatically filters whitespace messages") {
//         CHECK(engine.isLogFiltered(emptyLog) == true);
//     }
// }

// TEST_CASE("ConsoleEngine Text Generation (Copy Feature)", "[ConsoleEngine]") {
//     Logger mockLogger;
//     ConsoleEngine engine;
//     engine.initialize(&mockLogger);
    
//     // Inject logs into the mock sink
//     LogEntry log1;
//     log1.level = LogLevel::INFO;
//     log1.src = "System";
//     log1.msg = "Initialization complete.";
//     log1.category = LogCategory::SYSTEM;

//     LogEntry log2;
//     log2.level = LogLevel::LOG_ERROR;
//     log2.src = "Renderer";
//     log2.msg = "Shader failed to compile.";
//     log2.category = LogCategory::SHADER;

//     mockLogger.getConsoleSinkPtr()->addMockLog(log1);
//     mockLogger.getConsoleSinkPtr()->addMockLog(log2);

//     SECTION("Generates full text string correctly") {
//         std::string result = engine.getFilteredLogText();
        
//         // Check for presence of formatted strings
//         CHECK_THAT(result, Catch::Matchers::Contains("[INFO: System] Initialization complete."));
//         CHECK_THAT(result, Catch::Matchers::Contains("[ERROR: Renderer] Shader failed to compile."));
//     }

//     SECTION("Respects filters when generating text") {
//         // Disable Errors
//         engine.getToggles().isShowError = false;

//         std::string result = engine.getFilteredLogText();

//         // Info should exist
//         CHECK_THAT(result, Catch::Matchers::Contains("Initialization complete."));
//         // Error should NOT exist
//         CHECK_THAT(result, !Catch::Matchers::Contains("Shader failed to compile."));
//     }
    
//     SECTION("Clear logs action works") {
//         engine.executeBtnAction(ConsoleActions::CLEAR);
//         CHECK(mockLogger.getConsoleSinkPtr()->getLogs().empty());
//         CHECK(engine.getFilteredLogText() == "");
//     }
// }