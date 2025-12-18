#include "FileSink.hpp"
#include <deque> 

class ConsoleSink : FileSink {
    public: 
        void addLog(const LogEntry& entry) override; 
        const std::deque<LogEntry&> getLogs() const; 
    private: 
    std::deque<LogEntry> logs; 
    const size_t MAX_HISTORY = 1024; 
}; 