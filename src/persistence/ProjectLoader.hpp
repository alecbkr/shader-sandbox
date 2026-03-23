#pragma once

class ModelCache;
class MaterialCache;
class AssimpImporter;

struct Project;

struct ProjectLoader {
    static int version;

    static bool load(Project& project);
    static void save(Project& project, ModelCache* modelCachePtr, MaterialCache* materialCachePtr);
};
