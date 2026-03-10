    #pragma once
#include <string_view>
#include <unordered_map>
#include <memory>
#include <functional>
#include <deque>
#include "logging/ConsoleSink.hpp"
#include "logging/Logger.hpp"

class Logger;

namespace ConsoleActions {
    inline constexpr std::string_view CLEAR = "clear";
    inline constexpr std::string_view AUTO_SCROLL = "autoscroll";
    inline constexpr std::string_view COLLAPSE_LOGS = "collapse_logs";
    inline constexpr std::string_view OPEN_LOG_HISTORY = "open_log_history"; 
    inline constexpr std::string_view COPY_LOGS = "copy_logs"; 
    inline constexpr std::string_view SHOW_ERRORS = "show_errors";
    inline constexpr std::string_view SHOW_WARNINGS = "show_warnings";
    inline constexpr std::string_view SHOW_INFO = "show_info";
    inline constexpr std::string_view SHOW_SHADER = "show_shader";
    inline constexpr std::string_view SHOW_SYSTEM = "show_system";
    inline constexpr std::string_view SHOW_ASSETS = "show_assets";
    inline constexpr std::string_view SHOW_OTHER = "show_other";
}

struct ConsoleToggles {
    // Log Interaction
    bool isAutoScroll = true;
    bool isCollapsedLogs = false;
    
    // Log Levels
    bool isShowError = true;
    bool isShowWarning = true;
    bool isShowInfo = true;
    
    // Log Sources
    bool isShowShader = true;
    bool isShowSystem = true;
    bool isShowAssets = true;
    bool isShowUI = true;
    bool isShowOther = true;
}; 

class ConsoleEngine {
public:
    ConsoleEngine() = default;

    bool initialize(Logger* _loggerPtr);
    
    void registerButton(std::string_view name, std::function<void()> callbackFn);
    void registerToggle(std::string_view name, std::function<void(bool)> callbackFn);

    const std::deque<LogEntry>& getLogs() const;
    void executeBtnAction(std::string_view name);
    ConsoleToggles& getToggles();
    std::string getFilteredLogText() const;
    bool isLogFiltered(const LogEntry& log) const; 

private:
    bool initialized = false;
    std::deque<LogEntry> logs;
    ConsoleToggles toggles;
    std::shared_ptr<ConsoleSink> logSrc = nullptr;
    Logger* loggerPtr; 
    std::unordered_map<std::string_view, std::function<void()>> btnRegistry;
    std::unordered_map<std::string_view, std::function<void(bool)>> toggleRegistry;
    void openLogFile(const std::string logPath); 
};