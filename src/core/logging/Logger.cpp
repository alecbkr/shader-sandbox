#include "core/logging/Logger.hpp"
#include "core/logging/FileSink.hpp"
#include "core/logging/StdoutSink.hpp"

#define DEFAULT_SINKS LoggerInitialization::CONSOLE_FILE_STDOUT

Logger::Logger() {
    sinks.clear();
    consoleSinkPtr = nullptr;
    initialized = false;
}

bool Logger::initialize(){
    Logger::consoleSinkPtr = std::make_shared<ConsoleSink>();
    Logger::addSink(consoleSinkPtr);

    switch (DEFAULT_SINKS) {
        case LoggerInitialization::CONSOLE_FILE_STDOUT:
            Logger::addSink(std::make_shared<FileSink>());
            Logger::addSink(std::make_shared<StdoutSink>());
            break;
        
        case LoggerInitialization::CONSOLE_FILE:
            Logger::addSink(std::make_shared<FileSink>());
            break;
        
        case LoggerInitialization::CONSOLE_STDOUT:
            Logger::addSink(std::make_shared<StdoutSink>());
            break;
        
        case LoggerInitialization::CONSOLE_ONLY:
            break;

        default:
            std::cout << "Invalid Logger initialization setting!" << std::endl;
            return false;
    }

    Logger::initialized = true;
    return true;
}

void Logger::addSink(std::shared_ptr<LogSink> sink) {
    sinks.push_back(sink); 
}

void Logger::removeSink(std::shared_ptr<LogSink> sink) {
    std::erase(sinks, sink);    
}

void Logger::addLog(LogLevel level, std::string src, std::string msg, std::string additional, int lineNum) {
    if (!initialized) {
        std::cout << "Attempting to add log without initializing the logger!" << std::endl;
        return;
    };
    
    LogEntry entry; 
    entry.level = level; 
    entry.src = src; 
    entry.msg = msg; 
    entry.additional = additional; 
    entry.lineNum = lineNum; 

    // dispatch logs to included sinks 
    for(auto& sink : sinks) {
        sink->addLog(entry); 
    }

    // TODO: This code shouldn't live in the logger. We can add a flag in the logger like "hadCritialError" and check for that every frame
    // but the logger shouldn't be able to shut down the whole application, this will stip deconstructors and not allow us to shutdown gracefully.
    // if(level == LogLevel::CRITICAL) {
    //     exit(1); 
    // }
}

std::shared_ptr<ConsoleSink> Logger::getConsoleSinkPtr() {
    if (!initialized) return nullptr;
    return Logger::consoleSinkPtr;
}