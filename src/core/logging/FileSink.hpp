#pragma once
#include <fstream>
#include "LogSink.hpp"


class FileSink : public LogSink {
    public: 
    explicit FileSink(const std::string &path); 
    void addLog(const LogEntry& entry) override; 

    private: 
    std::ofstream file;  
    size_t maxLogSize; 
}; 
