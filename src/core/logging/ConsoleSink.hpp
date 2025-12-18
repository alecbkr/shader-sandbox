#include "FileSink.hpp"
#include <vector> 

class ConsoleSink : FileSink {
    public: 
        void addLog(const LogEntry& entry) override; 
        const std::vector<LogEntry&> getLogs() const; 
    private: 
    std::vector<LogEntry> logs; 
}; 