#include <iostream>
#include "LogSink.hpp"
#include <sstream>  // might want to change to format if we're using c++20

// used to print the colors of different logs in stdout
// TODO: add bolding and other things 
namespace {
    constexpr std::string_view BG_RED = "\033[97;41m";     
    constexpr std::string_view TEXT_ERROR = "\e[31m";             
    constexpr std::string_view TEXT_WARNING = "\e[35m";            
    constexpr std::string_view TEXT_INFO = "\e[32m";
    constexpr std::string_view RESET_COLORS = "\033[0m";
               
}; 

class StdoutSink : public LogSink {
    public: 
        StdoutSink() : LogSink () {};
        void addLog(const LogEntry& entry) override;  
}; 