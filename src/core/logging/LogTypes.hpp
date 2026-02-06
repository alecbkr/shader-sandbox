#include <string>
#include <string_view>
#include <sstream>    

enum class LogLevel {
    CRITICAL = 0, 
    LOG_ERROR = 1,
    WARNING = 2,
    INFO = 3
}; 

struct LogEntry {
    LogLevel level;
    std::string src;
    std::string msg;
    std::string additional; 
    int lineNum;       
};

// struct ColorDef {
//     float r, g, b, a;               // standard decimal representation 

//     // Convert to Ansi for stdout
//     std::string toAnsi() const {
//         int newR = static_cast<int>(r * 255); 
//         int newG = static_cast<int>(g * 255); 
//         int newB = static_cast<int>(b * 255); 
//         return std::sstream("")
//     }
// }

