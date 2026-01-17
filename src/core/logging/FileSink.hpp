#pragma once
#include <fstream>
#include "LogSink.hpp"
#include <filesystem>

// TODO: implement some way of cache clearing to remove old log files (on startup or after a few days)
class FileSink : public LogSink {
    public: 
    static constexpr std::string_view DEFAULT_LOG_PATH = "./src/core/logging/logs";  // might want to change this path later 
    FileSink(const std::string &log_dir = DEFAULT_LOG_PATH.data()); 
    ~FileSink(); 
    void addLog(const LogEntry& entry) override; 
    private:    
    const size_t MAX_LOG_FILE_SIZE = 2 * 100;        // max file size for a single log file (2mb) 
    const size_t MAX_LOGS_SIZE = 50 * 100;           // max size of logs all-together (50mb)
    std::ofstream activeLogFile;
    std::filesystem::path log_dir; 

    std::filesystem::path currLogFilePath; 
    int logIdx = 0; 

    void rotateLogFile(); 
    int findNextLogIndex();    
    void clearOldLog();  
    void getAllLogsSize(); 
}; 
