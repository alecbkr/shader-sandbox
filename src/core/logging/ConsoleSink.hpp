#pragma once
#include "LogSink.hpp"
#include <deque> 

class ConsoleSink : public LogSink {
    public: 
        void addLog(const LogEntry& entry) override; 
        const std::deque<LogEntry>& getLogs() const; 
        void clearLogs(); 
    private: 
    std::deque<LogEntry> items; 
    const size_t MAX_HISTORY = 1024; 
}; 