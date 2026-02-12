#include <string_view>
#include <string> 
#include <source_location>
#include <array>

enum class LogCategory {
    SHADER, 
    SYSTEM, 
    UI, 
    ASSETS, 
    OTHER, 
};

// helper to get categorize what system each file is apart of 
class LogClassifier {
    public:
    static std::string categoryToString(LogCategory cat);
    static LogCategory categorize(const std::source_location& fileLoc);
};