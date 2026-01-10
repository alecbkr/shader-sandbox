#include "FileSink.hpp"
#include <iostream>
#include <chrono>
#include <format>

FileSink::FileSink() : LogSink() {
    logFile.open(logPath, std::ios::out | std::ios::app); 
}

FileSink::~FileSink() {
    if(logFile.is_open()) {
        logFile.close(); 
    }
}

void FileSink::addLog(const LogEntry& entry) {
    if(!logFile.is_open()) {
        std::cerr << "Failed to open file" << std::endl; 
        return; 
    }

    std::stringstream alert; 

    switch (entry.level) {
        case    LogLevel::CRITICAL:  alert << "CRITICAL: ";  break; 
        case    LogLevel::ERROR:     alert  << "ERROR: ";    break; 
        case    LogLevel::WARNING:   alert  << "WARNING: ";  break; 
        case    LogLevel::INFO:      alert << "INFO: ";      break;
        default:                     alert << "ANOMALY: ";   break; 
    }    

    auto now = std::chrono::system_clock::now(); 
    std::string timestamp = std::format("{:%Y-%m-%d %H:%M:%SZ}", now); 

    // std::string newLog = '[' +  timestamp + "] " + "[" + alert.str() + entry.src + "] " + entry.msg + entry.additional + '\n';   
   std::string newLog = std::format("[{}] [{}{}] {}{}\n", 
        timestamp, 
        alert.str(), 
        entry.src, 
        entry.msg, 
        entry.additional
    );

    logFile << newLog << std::flush; 
}