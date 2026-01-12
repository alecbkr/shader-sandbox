#pragma once 
#include <memory>
#include <vector> 
#include "LogSink.hpp"

enum class LoggerInitialization {
    CONSOLE_FILE_STDOUT = 0, 
    CONSOLE_ONLY = 1,
    FILE_ONLY = 2,
    STDOUT_ONLY = 3,
    CONSOLE_FILE = 4,
    CONSOLE_STDOUT = 5,
    FILE_STDOUT = 6
}; 

class Logger {
    public: 
    Logger() = delete; 

    static bool initialize(LoggerInitialization initSetting);
    
    static void addLog(LogLevel level, std::string src, std::string msg, std::string additional = "", int lineNum = -1); 

    static void addSink(std::shared_ptr<LogSink> sink); 

    static void removeSink(std::shared_ptr<LogSink> sink);
    
    private:
    static std::vector<std::shared_ptr<LogSink>> sinks;
    static bool initialized;
}; 