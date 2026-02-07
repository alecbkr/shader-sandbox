#pragma once 
#include <memory>
#include <vector> 
#include "LogSink.hpp"
#include "core/logging/ConsoleSink.hpp"

enum class LoggerInitialization {
    CONSOLE_FILE_STDOUT = 0, 
    CONSOLE_FILE = 1,
    CONSOLE_STDOUT = 2,
    CONSOLE_ONLY = 3
}; 

class Logger {
public: 
    Logger();

    bool initialize();
    void addLog(LogLevel level, std::string src, std::string msg, std::string additional = "", int lineNum = -1); 
    void addSink(std::shared_ptr<LogSink> sink); 
    void removeSink(std::shared_ptr<LogSink> sink);
    std::shared_ptr<ConsoleSink> getConsoleSinkPtr();
    
private:
    std::vector<std::shared_ptr<LogSink>> sinks;
    std::shared_ptr<ConsoleSink> consoleSinkPtr = nullptr;
    bool initialized = false;
}; 