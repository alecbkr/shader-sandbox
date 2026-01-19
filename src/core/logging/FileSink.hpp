#pragma once
#include <fstream>
#include "LogSink.hpp"
#include <filesystem>

// TODO: implement some way of cache clearing to remove old log files (on startup or after a few days)
class FileSink : public LogSink {
    public: 
    FileSink(const std::filesystem::path& log_dir = "../src/core/logging/logs");    // make this path more exportable on release 
    ~FileSink(); 
    void addLog(const LogEntry& entry) override; 
    private:    
    const size_t MAX_LOG_FILE_SIZE = 2 * 1000 * 1000;        // max file size for a single log file (2mb) 
    const size_t MAX_LOGS_SIZE = 50 * 1000 * 1000;           // max size of logs all-together (50mb)
    std::ofstream activeLogFile;
    std::filesystem::path log_dir; 

    std::filesystem::path currLogFilePath; 
    int logIdx = 0; 

    void rotateLogFile(); 
    int findNextLogIndex();    
    void clearOldLog();  
    void getAllLogsSize(); 
}; 
