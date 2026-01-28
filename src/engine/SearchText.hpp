#pragma once 
#include "imgui.h"
#include <string>
#include <vector>
#include <functional> 
#include <regex> 
#include <algorithm>
#include <deque>


// bool flags that control what you want to appear in the search widget 
enum class SearchUIFlags {
    NONE = 0, 
    CASE_SENSITIVE = 1 << 0,
    MATCH_WHOLE_WORD = 1 << 1, 
    REGEX = 1 << 2, 
    REPLACE = 1 << 3,            
    DEFAULT = CASE_SENSITIVE | MATCH_WHOLE_WORD, // console implementation 
    ADVANCED = CASE_SENSITIVE | MATCH_WHOLE_WORD | REGEX // add replace later 
}; 

// enforces enum usage to prevent using different enums or bools with SearchUIFlags
inline SearchUIFlags operator | (SearchUIFlags a, SearchUIFlags b) {
    return static_cast<SearchUIFlags>(static_cast<int>(a) | static_cast<int>(b)); 
}
inline bool operator & (SearchUIFlags a, SearchUIFlags b) {
    return (static_cast<int>(a) & static_cast<int>(b)) != 0; 
}

// find widget note: that highlighting text must be done inside the implementation of the desired widget
class SearchText {
    public: 
    struct Match {
        int itemIdx; 
        int charIdx; 
        int length; 
    }; 

    SearchText() = default;
    bool useRegex = false; 
    bool caseSensitive = false; 
    bool matchWholeWord = false; 
    bool showReplace = false; 

    bool drawSearchUI(std::function<void()> onReplaceClick = nullptr);     // UI for find 
    bool GetisDirty() const {return isDirty;}                              // User updated the search bar 
    void setDirty() {isDirty = true;}
    bool hasQuery() const {return inputBuffer[0] != '\0'; }

    // Where the actual text filtering will take place 
    template <typename Container, typename Func> 
    void updateMatches(const Container &data, Func textExtractor) {
        matches.clear(); 
        std::string query = inputBuffer; 

        if(query.empty()) {
            clearResultState(); 
            return; 
        }

        try {
            if(useRegex && (flags & SearchUIFlags::REGEX)) {
                auto regexFlags = std::regex_constants::ECMAScript;
                if (!caseSensitive) {
                    regexFlags |= std::regex_constants::icase;
                } 

                // used when match whole word and regex is enable to wrap it
                std::string finalQuery = query; 
                if (matchWholeWord) {
                    finalQuery = "\\b" + finalQuery + "\\b"; 
                }

                std::regex regexPattern(finalQuery, regexFlags); 

                for(int i = 0; i < data.size(); ++i) {
                    std::string line = textExtractor(data[i]); 
                    auto words_begin = std::sregex_iterator(line.begin(), line.end(), regexPattern); 
                    auto words_end = std::sregex_iterator(); 

                    for (std::sregex_iterator reg = words_begin; reg != words_end; ++reg) {
                        matches.push_back({i, (int)reg->position(), (int)reg->length()}); 
                    }
                }
            }

            else {
                std::string searchQuery = query; 
                
                if (!caseSensitive) {
                    std::transform(searchQuery.begin(), searchQuery.end(), searchQuery.begin(), ::tolower); 
                }

                for (int i = 0; i < data.size(); ++i) {
                    std::string line = textExtractor(data[i]); 
                    std::string searchLine = line; 

                    if(!caseSensitive) {
                        std::transform(searchLine.begin(), searchLine.end(), searchLine.begin(), ::tolower);
                    }

                    size_t pos = searchLine.find(searchQuery, 0); 
                    while(pos != std::string::npos) {
                        bool isMatch = true; 

                        // check the whole word's bounds 
                        if (matchWholeWord) {
                            if (pos > 0) {
                                char c = searchLine[pos - 1]; 
                                if (isalnum(c) || c == '_') 
                                    isMatch = false; 
                            }

                            if (isMatch && (pos + searchQuery.length() < searchLine.length())) {
                                char c = searchLine[pos + searchQuery.length()]; 
                                if (isalnum(c) || c == '_') 
                                    isMatch = false;     
                            }
                            
                        }
                        if (isMatch) matches.push_back({i, (int)pos, (int)searchQuery.length() });
                
                        pos = searchLine.find(searchQuery, pos + 1); 
                    }
                }
            }
            statusMessage = ""; 
        }
        catch (const std::regex_error&) {
            statusMessage = "Invalid Regex"; 
            matches.clear(); 
        }

        if (!matches.empty()) {
            currentMatchIdx = 0; 
            requestScroll = true; 
        } else {
            currentMatchIdx = -1;
        }
        isDirty = false; 
    }

    // TODO: implement replace logic later for other componenets 
    template <typename T> 
    bool replaceCurrent (std::vector<T>& data, std::function<std::string(const T&)> getter, std::function<void(int, std::string)> setter) {
        return false; 
    }

    bool checkAndClearScrollRequest() {
        if (requestScroll) {
            requestScroll = false; 
            return true; 
        }
        return false; 
    }

    bool isItemActiveMatch(int itemIndex) const {
        return currentMatchIdx != -1 && matches[currentMatchIdx].itemIdx == itemIndex; 
    }

    const Match& getActiveMatch() const {
        return matches[currentMatchIdx]; 
    }

    void setSearchFlag(SearchUIFlags newFlags); 

    private: 
    void clearResultState() {
        matches.clear(); 
        currentMatchIdx = -1; 
        statusMessage = ""; 
        isDirty = false; 
    }

    char inputBuffer[512] = ""; 
    char replaceBuffer[512] = "";
    SearchUIFlags flags = SearchUIFlags::DEFAULT;
    std::vector<Match> matches; 
    int currentMatchIdx = -1; 
    bool requestScroll = false; 
    bool isDirty = false; 
    std::string statusMessage = ""; 
};