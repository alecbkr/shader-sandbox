#pragma once 
#include <vector>
#include <string>

namespace shbx {
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
        int lineNum;       
    };

    class Logger {
        public: 
        Logger() = delete; 

        static void addLog(LogLevel level, const char *src, const char *msg, int lineNum = -1); 
        static void addLog(LogLevel level, const char *src, const std::string msg, int lineNum = -1); 
        static void clearLogs(); 
        static void setAbortWhen(LogLevel level); 
        static const std::vector<LogEntry>& getLogs(); 

        private:
        static std::vector<LogEntry> logs;     
        static LogLevel abortWhen;     // used to revert back to default shade 
    };
}; 