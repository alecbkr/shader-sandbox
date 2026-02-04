#pragma once 
#include <memory>
#include <vector> 
#include "LogSink.hpp"
#include "core/logging/ConsoleSink.hpp"
#include <source_location>
#include <string_view>


enum class LoggerInitialization {
    CONSOLE_FILE_STDOUT = 0, 
    CONSOLE_FILE = 1,
    CONSOLE_STDOUT = 2,
    CONSOLE_ONLY = 3
}; 

class Logger {
public: 
    Logger() = delete; 
    static bool initialize(LoggerInitialization initSetting);
    static void addLog(LogLevel level, std::string src, std::string msg, std::string additional = "", int lineNum = -1,  const std::source_location& file_location = std::source_location::current()); 
    static void addSink(std::shared_ptr<LogSink> sink); 
    static void removeSink(std::shared_ptr<LogSink> sink);
    static std::shared_ptr<ConsoleSink> getConsoleSinkPtr();
    
private:
    static LogLevel abortWhen; 
    static std::vector<std::shared_ptr<LogSink>> sinks;
    static std::shared_ptr<ConsoleSink> consoleSinkPtr;
    static constexpr std::string_view toRelativePath(const char* path, std::string_view prefix); 
    static bool initialized;
}; 