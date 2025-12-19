#include "Logger.hpp"

std::vector<std::shared_ptr<LogSink>> Logger::sinks; 

void Logger::addSink(std::shared_ptr<LogSink> sink) {
    sinks.push_back(sink); 
}

void Logger::removeSink(std::shared_ptr<LogSink> sink) {
    
}

void Logger::addLog(LogLevel level, std::string src, std::string msg, int lineNum = -1) {
    LogEntry entry; 
    entry.level = level; 
    entry.src = src; 
    entry.msg = msg; 
    entry.lineNum = lineNum; 

    // dispatch logs to included sinks 
    for(auto& sink : sinks) {
        sink->addLog(entry); 
    }

    // TODO: handle abort logic 
}