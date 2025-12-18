#include <iostream>
#include "LogSink.hpp"

class StdoutSink : LogSink {
    public: 
        void addLog(const LogEntry& entry) override; 
    private: 
}; 