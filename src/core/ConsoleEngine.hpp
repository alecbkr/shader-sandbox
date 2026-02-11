#pragma once
#include <sstream>
#include <string> 
#include <unordered_map>
#include <memory>
#include <functional>
#include "logging/ConsoleSink.hpp"
#include "logging/Logger.hpp"

struct ConsoleActions {
    static const std::string CLEAR;
    static const std::string AUTO_SCROLL;
    static const std::string COLLAPSE_LOGS;
    static const std::string SHOW_ERRORS; 
    static const std::string SHOW_WARNINGS; 
    static const std::string SHOW_INFO; 
    static const std::string SHOW_SHADER; 
    static const std::string SHOW_SYSTEM; 
    static const std::string SHOW_ASSETS; 
    static const std::string SHOW_OTHER;
};

struct ConsoleToggles {
    // Log Interaction
    bool isAutoScroll;
    bool isCollapsedLogs;
    
    // Log Levels
    bool isShowError;
    bool isShowWarning;
    bool isShowInfo;
    
    // Log Sources
    bool isShowShader;
    bool isShowSystem;
    bool isShowAssets;
    bool isShowUI; 
    bool isShowOther; 
}; 

class ConsoleEngine {
    public:
    // struct Cmd {
    //     // buttons
    //     static constexpr const char* CLEAR       = "clear";

    //     // toggles 
    //     static constexpr const char* AUTO_SCROLL = "autoscroll";
    //     static constexpr const char* COLLAPSE_LOGS = "collapse_logs"; 
    //     static constexpr const char* SHOW_ERRORS = "show_errors";
    //     static constexpr const char* SHOW_INFO   = "show_info";
    //     static constexpr const char* SHOW_WARN   = "show_warnings";
    // }; 
    
    static bool initialize(Logger* _loggerPtr);
    // I set up like this to try and make adding and parsing through some sort of config file easier 
    static void registerButton(const std::string& name, std::function<void()> callbackFn); 
    static void registerToggle(const std::string& name, std::function<void(bool)> callbackFn); 
    static const std::deque<LogEntry> &getLogs(); 
    static void executeBtnAction(const std::string& name);
    static ConsoleToggles& getToggles();  

    private:
    static bool initialized; 
    static std::unordered_map<std::string, std::function<void()>> btnRegistry; 
    static std::unordered_map<std::string, std::function<void(bool)>> toggleRegistry; 
    static std::shared_ptr<ConsoleSink> logSrc; 
    static std::deque<LogEntry> logs; 
    static ConsoleToggles toggles; 
}; 