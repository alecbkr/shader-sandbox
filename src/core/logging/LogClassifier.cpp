#include "LogClassifier.hpp"
#include <algorithm>
#include <cctype>

// Helper class that identifies what group each file or function belongs to in the project for the logger 
// (e.g. hot reloader belongs to shader) 

namespace {
    struct CategoryRule {
        std::string_view pattern;
        LogCategory category; 
    };

    // Note this map does take in where the file belong to first, e.g. the ui under the ui/ will belong to ui even if 
    // they are assigned to something else later 
    constexpr std::array rules = {
        CategoryRule{"ui", LogCategory::UI}, 
        CategoryRule{"menu", LogCategory::UI}, 
        CategoryRule{"editor", LogCategory::UI}, 

        CategoryRule{"object", LogCategory::ASSETS}, 
        CategoryRule{"texture", LogCategory::ASSETS},
        CategoryRule{"model", LogCategory::ASSETS},
        CategoryRule{"mesh", LogCategory::ASSETS},
        CategoryRule{"material", LogCategory::ASSETS},


        CategoryRule{"shader", LogCategory::SHADER},
        CategoryRule{"render", LogCategory::SHADER},
        CategoryRule{"uniform", LogCategory::SHADER},    

        CategoryRule{"application", LogCategory::SYSTEM},
        CategoryRule{"app", LogCategory::SYSTEM},
        CategoryRule{"input", LogCategory::SYSTEM},
        CategoryRule{"logging", LogCategory::SYSTEM},
        CategoryRule{"engine", LogCategory::SYSTEM},
        CategoryRule{"event", LogCategory::SYSTEM},
        CategoryRule{"file", LogCategory::SYSTEM},
        CategoryRule{"timer", LogCategory::SYSTEM},
        CategoryRule{"camera", LogCategory::SYSTEM},
        CategoryRule{"hotreloader", LogCategory::SYSTEM},
        CategoryRule{"persistence", LogCategory::SYSTEM},
        CategoryRule{"platform", LogCategory::SYSTEM},
        CategoryRule{"window", LogCategory::SYSTEM},
        CategoryRule{"console", LogCategory::SYSTEM},
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
    // std::string_view filePath = fileLoc.file_name();

    // for (const auto& rule : rules) {
    //     if (filePath.find(rule.pattern) != std::string_view::npos) {
    //         return rule.category; 
    //     }
    // }

    // return LogCategory::OTHER; 
    return categorizeByString(fileLoc.file_name()); 
}
     
LogCategory LogClassifier::categorizeByString(std::string_view filePath) {
    std::string pathLower(filePath);
    std::transform(pathLower.begin(), pathLower.end(), pathLower.begin(), 
                   [](unsigned char c){ return std::tolower(c); });

    for (const auto& rule : rules) {
        if (pathLower.find(rule.pattern) != std::string::npos) {
            return rule.category; 
        }
    }

    return LogCategory::OTHER;
}
