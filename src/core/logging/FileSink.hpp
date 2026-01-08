#pragma once
#include <fstream>
#include <iostream>
#include <chrono>
#include <format>
#include "LogSink.hpp"

// TODO: implement some way of cache clearing to remove old log files (on startup or after a few days)
class FileSink : public LogSink {
    public: 
    FileSink() : LogSink () {};
    void addLog(const LogEntry& entry) override; 
    private:
}; 
