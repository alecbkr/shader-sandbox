#pragma once
#include <string>

struct Project;

struct ProjectLoader {
    static int version;

    static bool load(Project& project);
    static void save(const Project& project);
};

