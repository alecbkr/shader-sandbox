#include <string>
#include <string_view>
#include <sstream> 
#include "LogClassifier.hpp"   

enum class LogLevel {
    CRITICAL = 0, 
    ERROR = 1,
    WARNING = 2,
    INFO = 3
}; 

struct LogEntry {
    LogLevel level;
    std::string src;
    std::string msg;
    std::string additional; 
    LogCategory category; 
    std::string fileName; 
    int lineNum;       
};

