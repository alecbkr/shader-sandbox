#include "ConsoleSink.hpp"

void ConsoleSink::addLog(const LogEntry& entry) {
    int idx = std::min((int)entry.level, 4); 
    std::stringstream alert; 
}