#pragma once
#include <sstream>
#include <string> 
#include <unordered_map>
#include <memory>
#include <functional>
#include "logging/ConsoleSink.hpp"

class ConsoleEngine {
    public:
    struct Cmd {
        // buttons
        static constexpr const char* CLEAR       = "clear";

        // toggles 
        static constexpr const char* AUTO_SCROLL = "autoscroll";
        static constexpr const char* SHOW_ERRORS = "show_errors";
        static constexpr const char* SHOW_INFO   = "show_info";
        static constexpr const char* SHOW_WARN   = "show_warnings";
    }; 
    
    static bool initialize(std::shared_ptr<ConsoleSink> consoleSink); 
    static void registerButton(const std::string& name, std::function<void()> callbackFn); 
    static void registerToggle(const std::string& name, std::function<void(bool)> callbackFn); 
    static const std::deque<LogEntry> &getLogs(); 
    static void executeBtnAction(const std::string& name); 

    private:
    static bool initialized; 
    static std::unordered_map<std::string, std::function<void()>> btnRegistry; 
    static std::unordered_map<std::string, std::function<void(bool)>> toggleRegistry; 
    static std::shared_ptr<ConsoleSink> logSrc; 
    static std::deque<LogEntry> logs; 
    
}; 