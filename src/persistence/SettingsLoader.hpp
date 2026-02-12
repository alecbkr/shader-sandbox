#pragma once

struct AppSettings;

struct SettingsLoader {
    static int version;

    static bool load(AppSettings& settings);
    static void save(const AppSettings& settings);
};
