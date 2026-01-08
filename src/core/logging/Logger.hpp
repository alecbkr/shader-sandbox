#pragma once 
#include <memory>
#include <vector> 
#include "LogSink.hpp"

class Logger {
    public: 
    Logger() = delete; 
    
    static void addLog(LogLevel level, std::string src, std::string msg, int lineNum); 

    static void addSink(std::shared_ptr<LogSink> sink); 

    static void removeSink(std::shared_ptr<LogSink> sink);
    
    static void getLogs(); 

    private:
    static std::vector<std::shared_ptr<LogSink>> sinks; 
}; 