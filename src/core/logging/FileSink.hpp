#pragma once
#include <fstream>
#include "LogSink.hpp"
#include <filesystem>
#include <string> 



class FileSink : public LogSink {
    public: 
    FileSink(const std::filesystem::path& log_dir);    
    ~FileSink(); 
    void addLog(const LogEntry& entry) override; 
    const std::filesystem::path getLogDir() const; 
    static std::filesystem::path GetProjectLogDirectory(const std::string& appName, const std::string& projectName);

    protected:    
    struct LogFileInfo {
        std::filesystem::path path; 
        uintmax_t size; 
        std::filesystem::file_time_type lastWritten; 
        int idx; 
    }; 

    uintmax_t MAX_LOG_FILE_SIZE = 2 * 1000 * 1000;        // max file size for a single log file (2mb) 
    uintmax_t MAX_LOGS_SIZE = 50 * 1000 * 1000;           // max size of logs all-together (50mb)
    std::ofstream activeLogFile;
    std::filesystem::path log_dir; 

    std::filesystem::path currLogFilePath; 
    int logIdx = 0; 
    static constexpr int MAX_LOG_FILES_COUNT = 64; 

    void rotateLogFile(); 
    int findNextLogIndex();    
    void cleanLogs(); 
    void compareFileWriteTimes(LogFileInfo &log1, LogFileInfo &log2);  
}; 
