#include "LogClassifier.hpp"

// Helper class that identifies what group each file or function belongs to in the project for the logger 
// (e.g. hot reloader belongs to shader) 

namespace {
    struct CategoryRule {
        std::string_view pattern;
        LogCategory category; 
    };

    // TODO: go through each file and separate and look at func ccall to see if they need to go to another category 
    // will be used to try to map file names to their respected categories 
    constexpr std::array rules = {
        CategoryRule{ "Shader", LogCategory::SHADER}, 
        CategoryRule{ "Render", LogCategory::SHADER}, 

        CategoryRule{ "logging/", LogCategory::SYSTEM},

        // these two might be redundant 
        CategoryRule{ "Sink", LogCategory::SYSTEM},
        CategoryRule{ "Logger", LogCategory::SYSTEM},

        CategoryRule{ "Timer", LogCategory::SYSTEM},
        CategoryRule{ "Event", LogCategory::SYSTEM},
        CategoryRule{ "Camera", LogCategory::SYSTEM},
        CategoryRule{ "HotReload", LogCategory::SYSTEM},
        CategoryRule{ "Application", LogCategory::SYSTEM},

        CategoryRule{"ui/", LogCategory::UI}, 
         CategoryRule{"Window/", LogCategory::UI}, 

        CategoryRule{"Texture", LogCategory::ASSETS}, 
        CategoryRule{"Registry", LogCategory::ASSETS}, 
        CategoryRule{"File", LogCategory::ASSETS}, 
        CategoryRule{"Uniform", LogCategory::ASSETS}, 
    }; 
}

std::string LogClassifier::categoryToString(LogCategory cat) {
    switch (cat) {
        case LogCategory::SHADER: return "SHADER"; 
        case LogCategory::SYSTEM: return "SYSTEM"; 
        case LogCategory::UI: return "UI"; 
        case LogCategory::ASSETS: return "ASSETS"; 
        case LogCategory::OTHER: return "OTHER"; 
    }
    return "OTHER"; 
}

// takes incoming file and organizes it to the corresponding section
LogCategory LogClassifier::categorize(const std::source_location& fileLoc) {
    std::string_view filePath = fileLoc.file_name();

    for (const auto& rule : rules) {
        if (filePath.find(rule.pattern) != std::string_view::npos) {
            return rule.category; 
        }
    }

    return LogCategory::OTHER; 
}
     
