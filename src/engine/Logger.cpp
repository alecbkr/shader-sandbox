// #include "Logger.hpp"

// namespace shbx {
//     std::vector<LogEntry> Logger::logs; 
//     LogLevel Logger::abortWhen = LogLevel::CRITICAL; // Might need to adjust this depending on how we want the program to crash

//     void Logger::addLog(shbx::LogLevel level, const char *src, const char *msg, int lineNum) {
//         LogEntry newLog = {.level = level, .src = src, .msg = msg, .lineNum = lineNum};
//         logs.push_back(newLog);
        
//         // TODO: if we need to store the logs in a file 

//         // TODO: if we want to output to stdout aswell 

//         // TODO: handle abort logic (throw exception or crash)
//     }

//     void Logger::addLog(shbx::LogLevel level, const char *src, const std::string msg, int lineNum) {
//         LogEntry newLog = {.level = level, .src = src, .msg = msg, .lineNum = lineNum}; 
//         logs.push_back(newLog); 

//         // TODO: if we need to store the logs in a file 

//         // TODO: if we want to output to stdout aswell 

//         // TODO: handle abort logic (throw exception or crash)
//     }

//     void Logger::clearLogs() {
//         logs.clear(); 
//     }

//     void Logger::setAbortWhen(LogLevel level) {
//         Logger::abortWhen = level; 
//     }

//     const std::vector<LogEntry>& Logger::getLogs() {
//         return Logger::logs; 
//     }
// }; 