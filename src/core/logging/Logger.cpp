#include "core/logging/Logger.hpp"
#include "core/logging/FileSink.hpp"
#include "core/logging/StdoutSink.hpp"

std::shared_ptr<ConsoleSink> Logger::consoleSinkPtr = nullptr;

std::vector<std::shared_ptr<LogSink>> Logger::sinks;
bool Logger::initialized = false;
LogLevel Logger::abortWhen = LogLevel::CRITICAL; 

bool Logger::initialize(LoggerInitialization initSetting){
    Logger::consoleSinkPtr = std::make_shared<ConsoleSink>();
    Logger::addSink(consoleSinkPtr);

    switch (initSetting) {
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

    if(level == abortWhen){
        exit(1); 
    }
}

std::shared_ptr<ConsoleSink> Logger::getConsoleSinkPtr() {
    if (!initialized) return nullptr;
    return Logger::consoleSinkPtr;
}