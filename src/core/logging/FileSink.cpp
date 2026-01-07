#include "FileSink.hpp"

void FileSink::addLog(const LogEntry& entry) {
    int idx = std::min((int)entry.level, 4); // clamp the index to avoid out-of-bounds
    std::stringstream alert; 

    switch (entry.level) {
        case    LogLevel::CRITICAL:  alert << "CRITICAL: ";  break; 
        case    LogLevel::ERROR:     alert  << "ERROR: ";    break; 
        case    LogLevel::WARNING:   alert  << "WARNING: ";  break; 
        case    LogLevel::INFO:      alert << "INFO: ";      break;
        default:                     alert << "ANOMALY: ";   break; 
    }    

    std::string newLog = alert.str() + entry.msg;   
    
    std::ofstream file("logs.txt"); 

    if(file.is_open()) {
        file << newLog; 
        file.close(); 
    } else {
        // TODO: try to output error msg somewhere else 
        std::cerr << "Failed to open file" << std::endl; 
    }
}