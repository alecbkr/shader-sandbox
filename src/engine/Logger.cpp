#include "Logger.hpp"

shbx::Logger::Logger(){}

void shbx::Logger::addLog(ErrLevel level, const char *src, const char *msg, int lineNum = -1) {
    LogEntry newLog = {.level = level, .src = src, .msg = msg, .lineNum = lineNum};
    logs.push_back(newLog); 
}

void shbx::Logger::clearLogs() {
    logs.clear(); 
}

std::vector<const LogEntry> shbx::Logger::getLogs() {
    return logs; 
}