#pragma once
#include <fstream>
#include "LogSink.hpp"
#include <filesystem>

struct LogFileInfo {
    std::filesystem::path path; 
    uintmax_t size; 
    std::filesystem::file_time_type lastWritten; 
}; 

class FileSink : public LogSink {
    public: 
    FileSink(const std::filesystem::path& log_dir = "../src/core/logging/logs");    // make this path more exportable on release 
    ~FileSink(); 
    void addLog(const LogEntry& entry) override; 
    private:    
    static constexpr size_t MAX_LOG_FILE_SIZE = 2 * 1000 * 1000;        // max file size for a single log file (2mb) 
    static constexpr size_t MAX_LOGS_SIZE = 50 * 1000 * 1000;           // max size of logs all-together (50mb)
    std::ofstream activeLogFile;
    std::filesystem::path log_dir; 

    std::filesystem::path currLogFilePath; 
    int logIdx = 0; 
    static constexpr int MAX_LOG_AMOUNT = 64; 

    void rotateLogFile(); 
    int findNextLogIndex();    
    void clearOldLog();
    void compareFileWriteTimes(LogFileInfo &log1, LogFileInfo &log2);  
}; 
