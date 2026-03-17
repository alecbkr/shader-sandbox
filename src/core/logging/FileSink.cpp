#include "FileSink.hpp"
#include <iostream>
#include <chrono>
#include <format>
#include <algorithm>
#include <climits>
#include <cstdlib>

FileSink::FileSink(const std::filesystem::path &log_dir) : log_dir(log_dir) {
    if(!std::filesystem::is_directory(log_dir)) {
        if(!std::filesystem::create_directories(log_dir)) {
            std::cerr << "Could not create directory for logger" << log_dir << std::endl; 
        }
    }
    logIdx = findNextLogIndex(); 
    rotateLogFile(); 
}

FileSink::~FileSink() {
    if(activeLogFile.is_open()) {
        activeLogFile.close(); 
    }
}

void FileSink::addLog(const LogEntry& entry) {
    // log file greater than its capped size (2MB)
    if (activeLogFile.is_open() && activeLogFile.tellp() > static_cast<std::streamoff>(MAX_LOG_FILE_SIZE)) {
        rotateLogFile();
    }

    std::stringstream alert; 
    switch (entry.level) {
        case    LogLevel::CRITICAL:  alert << "CRITICAL: ";  break; 
        case    LogLevel::LOG_ERROR:     alert  << "ERROR: ";    break; 
        case    LogLevel::WARNING:   alert  << "WARNING: ";  break; 
        case    LogLevel::INFO:      alert << "INFO: ";      break;
        default:                     alert << "ANOMALY: ";   break; 
    }    

    auto now = std::chrono::system_clock::now(); 
    std::string timestamp = std::format("{:%Y-%m-%d %H:%M:%SZ}", now); // time in utc

    std::string newLog = std::format("'{}' [{}::UTC] [{}{}] {}{}\n", 
        entry.fileName, 
        timestamp, 
        alert.str(), 
        entry.src, 
        entry.msg, 
        entry.additional
    );

    activeLogFile << newLog << std::flush;      
}

const std::filesystem::path FileSink::getLogDir() const {
    return log_dir; 
}


std::filesystem::path FileSink::GetProjectLogDirectory(const std::string& appName, const std::string& projectName) {
    // fallback, filesink will create the directory in the current directory  
    std::filesystem::path baseDir = std::filesystem::current_path() / "Logs";           
    

    #if defined(_WIN32)
        const wchar_t* localAppData = _wgetenv(L"LOCALAPPDATA"); 
        if (!localAppData) throw std::runtime_error("LOCALAPPDATA not set or invalid access");
        baseDir = std::filesystem::path(localAppData) / appName / "Logs";

    #elif defined(__APPLE__)
    // ~/Library/Logs/<AppName>
    const char* home = std::getenv("HOME");
    if (home) {
        baseDir = std::filesystem::path(home) / "Library" / "Logs" / appName;
    } 

    #else 
    //  ~/.local/state/<AppName>/log
    const char* xdgStateHome = std::getenv("XDG_STATE_HOME");
    if (xdgStateHome) {
        baseDir = std::filesystem::path(xdgStateHome) / appName / "log";
    } else {
        const char* home = std::getenv("HOME");
        if (home) {
            baseDir = std::filesystem::path(home) / ".local" / "state" / appName / "log";
        } 
    }
    #endif
    
    return baseDir / projectName;
}

// moves the file pointer to a new txt file when size exceeds max amount, too many logs in the folder, or we exceed 50mb in logs
void FileSink::rotateLogFile(){
    if(activeLogFile.is_open()) activeLogFile.close(); 
    
    // edge case of integer overflow for creating new logs 
    if (logIdx >= INT_MAX - 1) {
        try {
            std::filesystem::remove_all(log_dir); 
            std::filesystem::create_directories(log_dir); 
        } catch (...) {}

        logIdx = 0; 
    } else {
        logIdx ++; 
    }

    cleanLogs(); 

    std::string filename = "log_" + std::to_string(logIdx) + ".txt"; 
    currLogFilePath = log_dir / filename; 
    activeLogFile.open(currLogFilePath, std::ios::out | std::ios::trunc);
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

void FileSink::cleanLogs() {
    if (!std::filesystem::exists(log_dir)) return; 
    std::vector<LogFileInfo> logs; 
    size_t totalDirSize = 0; 

    for (const auto& entry : std::filesystem::directory_iterator(log_dir)) {
        if (entry.is_regular_file()) {
            std::string fileName = entry.path().filename().string(); 
            if(fileName.starts_with("log_") && fileName.ends_with(".txt")) {
                try {
                    uintmax_t fileSize = entry.file_size(); 
                    int idx = std::stoi(fileName.substr(4, fileName.length() - 8)); 
                    logs.push_back({entry.path(), fileSize, entry.last_write_time(), idx}); 
                    totalDirSize += fileSize; 
                } catch(...) {

                }
            }
        }
    }

    // sort the logs by when they were last written to
    std::sort(logs.begin(), logs.end(), [](const LogFileInfo& a, const LogFileInfo& b) {
        if (a.lastWritten != b.lastWritten) return a.lastWritten < b.lastWritten;
        else return a.idx < b.idx; 
    });

    size_t currCount = logs.size(); 

    // clear files until we have less than 64 logs and they meet the size constraints
    for (const auto& log : logs) {
        bool isCountHigh = currCount >= static_cast<size_t>(MAX_LOG_FILES_COUNT); 
        bool isSizeHigh = totalDirSize > MAX_LOGS_SIZE; 

        if (isCountHigh || isSizeHigh) {
            std::error_code ec; 
            std::filesystem::remove(log.path, ec); 

            if (!ec) {
                currCount --; 
                if (totalDirSize >= log.size) totalDirSize -= log.size; 
            }
        } 

        else {
            break; 
        }
    }
}