#pragma once 
#include "LogTypes.hpp"
#include <source_location>

// Abstract base class for different logging methods (stdout, .txt file, and debug console)
// Different sinks are the different logging destinations we can add 
class LogSink {
    public: 
    virtual ~LogSink() = default; 
    virtual void addLog(const LogEntry& entry) = 0; 
};