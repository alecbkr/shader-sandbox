#pragma once 
#include <memory>
#include <vector> 
#include "LogSink.hpp"

class Logger {
    public: 
    Logger() = delete; 
    
    static void addLog(LogLevel level, std::string src, std::string msg, int lineNum = -1); 

    static void addSink(std::shared_ptr<LogSink> sink); 

    static void removeSink(std::shared_ptr<LogSink> sink);
    
    private:
    static std::vector<std::shared_ptr<LogSink>> sinks; 
}; 