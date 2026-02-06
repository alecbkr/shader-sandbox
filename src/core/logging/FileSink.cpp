#include "FileSink.hpp"
#include <iostream>
#include <chrono>
#include <format>
#include <algorithm>

FileSink::FileSink(const std::filesystem::path &log_dir) : log_dir(log_dir), logIdx(findNextLogIndex()) {
    if(!std::filesystem::is_directory(log_dir)) {
        if(!std::filesystem::create_directories(log_dir)) {
            std::cerr << "Could not create directory for logger" << log_dir << std::endl; 
        }
    }
    rotateLogFile(); 
}

FileSink::~FileSink() {
    if(activeLogFile.is_open()) {
        activeLogFile.close(); 
    }
}

void FileSink::addLog(const LogEntry& entry) {
    
    if(activeLogFile.is_open() && activeLogFile.tellp() > MAX_LOG_FILE_SIZE) {
        rotateLogFile(); 
    }

    if(activeLogFile.tellp() > MAX_LOG_FILE_SIZE) {
        activeLogFile.close(); 
    }

    std::stringstream alert; 

    switch (entry.level) {
        case    LogLevel::CRITICAL:  alert << "CRITICAL: ";  break; 
        case    LogLevel::ERROR:     alert  << "ERROR: ";    break; 
        case    LogLevel::WARNING:   alert  << "WARNING: ";  break; 
        case    LogLevel::INFO:      alert << "INFO: ";      break;
        default:                     alert << "ANOMALY: ";   break; 
    }    

    auto now = std::chrono::system_clock::now(); 
    std::string timestamp = std::format("{:%Y-%m-%d %H:%M:%SZ}", now); 

    std::string newLog = std::format("'{}' [{}] [{}{}] {}{}\n", 
        entry.fileName, 
        timestamp, 
        alert.str(), 
        entry.src, 
        entry.msg, 
        entry.additional
    );

    activeLogFile << newLog << std::flush;      
}

void FileSink::rotateLogFile(){
    if(activeLogFile.is_open()) {
        activeLogFile.close(); 
    }

    logIdx++; 

    if (logIdx >= MAX_LOG_AMOUNT) {
        logIdx = 0; 
    }

    std::string filename = "log_" + std::to_string(logIdx) + ".txt"; 
    currLogFilePath = log_dir / filename; 

    activeLogFile.open(currLogFilePath, std::ios::out | std::ios::trunc);

    clearOldLog(); 
}

// detects the next index of the log by reading through each Log File in '/logs' to ensure that logging occurs at most recent file 
int FileSink::findNextLogIndex() {
    int maxIdx = -1; 

    if(!std::filesystem::exists(log_dir)) {
        std::cout << "Could not find directory" << std::endl; 
        return -1; 
    }; 
    
    for (const auto& log: std::filesystem::directory_iterator(log_dir)) {
        if(log.is_regular_file()) {
            std::string fileName = log.path().filename().string();
            
            if(fileName.starts_with("log_") && fileName.ends_with(".txt")){
                // substring extracting "logs" and ".txt" from each log file under '/logs'
                std::string logNum = fileName.substr(4, fileName.length() - 8); 

                try {
                    int idx = std::stoi(logNum); 
                    maxIdx = (maxIdx > idx) ? maxIdx : idx;       
                } catch(...) {
                    // sanity check that ignores any file that doesn't match expected 'logs_<int>.txt' format  
                }
            }
        }
    }

    return maxIdx; 
}

// Clears the oldest log in 'logs/' to make room for new logs 
void FileSink::clearOldLog(){
    if(!std::filesystem::exists(log_dir)) {
        std::cout << "Could not find directory" << log_dir << std::endl;  
        return; 
    }

    std::vector<LogFileInfo> logs; 
    std::uintmax_t totalLogSize = 0; // used to determine if we need to remove the oldest log file 
    
    // populate meta-data that we need to read for each log file (path, filesize and last write time)
    for (const auto& log : std::filesystem::directory_iterator(log_dir)) {
        if(log.is_regular_file()) {
            std::string fileName = log.path().filename().string(); 
            LogFileInfo newFileInfo{log.path(), log.file_size(), log.last_write_time()}; 
            logs.push_back(newFileInfo); 
            totalLogSize += log.file_size(); 
        }
    }

    if(totalLogSize < MAX_LOGS_SIZE) {
        return;  
    }

    std::sort (logs.begin(), logs.end(), [](const LogFileInfo& a, const LogFileInfo& b) {
        return a.lastWritten < b.lastWritten; 
    });

    // iterate through the logs vector to determine oldest log and remove one or more oldest log files
    for(const LogFileInfo &log : logs) {
        // attempt to remove log file 
        std::error_code ec; 
        try{
            std::filesystem::remove(log.path, ec);
            totalLogSize -= log.size; 
            std::cout << "cleared log file" << std::endl; 
        } catch(...) {
            std::cerr << "Could not clear log file" << std::endl; 
        }
    }
}