#pragma once
#include <fstream>
#include "LogSink.hpp"

const std::string logPath = "../src/core/logging/logs.txt";

// TODO: implement some way of cache clearing to remove old log files (on startup or after a few days)
class FileSink : public LogSink {
    public: 
    FileSink();
    ~FileSink(); 
    void addLog(const LogEntry& entry) override; 
    private:
    std::ofstream logFile; 
}; 
