#pragma once 
#include <nlohmann/json.hpp>

// defined struct here so that project loader doesn't have to pull everything from the console engine 
struct ConsoleToggles {
    bool isAutoScroll = true;
    bool isCollapsedLogs = false;
    bool isShowError = true;
    bool isShowWarning = true;
    bool isShowInfo = true;
    bool isShowShader = true;
    bool isShowSystem = true;
    bool isShowAssets = true;
    bool isShowUI = true;
    bool isShowOther = true;
};

// fallback safety with defined values from the struct in case project is missing current active settings 
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ConsoleToggles, 
    isAutoScroll, isCollapsedLogs, isShowError, isShowWarning, isShowInfo, 
    isShowShader, isShowSystem, isShowAssets, isShowUI, isShowOther)