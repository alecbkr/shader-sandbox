#pragma once 
#include <vector>
#include <string>
#include "Errorlog.hpp"

namespace shbx {
    struct LogEntry {
        ErrLevel level;
        std::string src;
        std::string msg;
        int lineNum;       
    };

    class Logger {
        public: 
        Logger(); 

        void addLog(ErrLevel level, const char *src, const char *msg, int lineNum); 
        void clearLogs(); 
        std::vector<const LogEntry> getLogs(); 

        private:
        std::vector<const LogEntry> logs;     
    };
}; 