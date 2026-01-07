#pragma once
#include <fstream>
#include <algorithm>        // for std::min 
#include "LogSink.hpp"

// TODO: implement some way of cache clearing to remove old log files (on startup or after a few days)
class FileSink : public LogSink {
    public: 
    explicit FileSink(const std::string &path); 
    void addLog(const LogEntry& entry) override; 

    private: 
    std::ofstream file;   
}; 
