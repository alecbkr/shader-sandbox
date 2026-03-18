#define CATCH_CONFIG_MAIN 
#include <catch2/catch_amalgamated.hpp>
#include "core/logging/FileSink.hpp"
#include <filesystem>
#include <thread>
#include <vector>
#include <string>

// I did use ai to generate these test and have gone through each one to validate them and tweak them 
namespace fs = std::filesystem;

// Helper to create a dummy log entry
LogEntry createEntry(LogLevel level, const std::string& msg) {
    return LogEntry{level, "test_file.cpp", "TestFunc", msg};
}

// Helper to fill a log file to force rotation
void fillLogFile(FileSink& sink, size_t targetBytes) {
    std::string largePayload(1024, 'A'); // 1KB string
    LogEntry entry = createEntry(LogLevel::INFO, largePayload);
    
    // We roughly estimate the wrapper overhead (timestamp etc) is negligible compared to payload
    size_t iterations = targetBytes / 1024; 
    for(size_t i = 0; i < iterations + 50; ++i) { // +50 for buffer safety
        sink.addLog(entry);
    }
}

TEST_CASE("FileSink Initialization", "[FileSink]") {
    fs::path testDir = "./test_logs_init";
    
    // Cleanup before starting
    if (fs::exists(testDir)) fs::remove_all(testDir);

    SECTION("Creates directory and first log file") {
        {
            FileSink sink(testDir);
            REQUIRE(fs::exists(testDir));
            REQUIRE(fs::is_directory(testDir));
            
            // Should create log_0.txt immediately
            REQUIRE(fs::exists(testDir / "log_0.txt"));
        }
    }

    // Cleanup
    fs::remove_all(testDir);
}

TEST_CASE("FileSink Rotation Logic", "[FileSink]") {
    fs::path testDir = "./test_logs_rotation";
    if (fs::exists(testDir)) fs::remove_all(testDir);

    SECTION("Rotates file when size exceeds limit (2MB)") {
        FileSink sink(testDir);
        
        // 1. Verify start at log_0
        REQUIRE(fs::exists(testDir / "log_0.txt"));
        REQUIRE_FALSE(fs::exists(testDir / "log_1.txt"));

        // 2. Fill log_0 past 2MB
        // 2MB = 2 * 10^6 bytes. 
        fillLogFile(sink, 2100000); 

        // 3. Verify log_1 is created
        REQUIRE(fs::exists(testDir / "log_1.txt"));
        
        // 4. Verify log_0 still exists (it shouldn't be deleted yet)
        REQUIRE(fs::exists(testDir / "log_0.txt"));
        
        // 5. Verify log_0 size is roughly capped (it might be slightly over 2MB due to the last write)
        REQUIRE(fs::file_size(testDir / "log_0.txt") >= 2000000);
    }

    fs::remove_all(testDir);
}

TEST_CASE("FileSink Persistence", "[FileSink]") {
    fs::path testDir = "./test_logs_persist";
    if (fs::exists(testDir)) fs::remove_all(testDir);

    SECTION("Finds next index on restart") {
        {
            FileSink sink(testDir);
            // Creates log_0.txt
        } 
        
        // Simulate restart
        {
            FileSink sink(testDir);
            // Should detect log_0 exists and create log_1
            REQUIRE(fs::exists(testDir / "log_1.txt"));
        }
        
        // Simulate another restart
        {
            FileSink sink(testDir);
            // Should detect log_1 exists and create log_2
            REQUIRE(fs::exists(testDir / "log_2.txt"));
        }
    }

    fs::remove_all(testDir);
}

TEST_CASE("FileSink Cleanup Constraints", "[FileSink]") {
    fs::path testDir = "./test_logs_cleanup";
    if (fs::exists(testDir)) fs::remove_all(testDir);
    fs::create_directories(testDir);

    // Manually populate the directory to simulate a "full" state
    // We create 64 small dummy files: log_0.txt ... log_63.txt
    for (int i = 0; i < 64; ++i) {
        std::ofstream ofs(testDir / ("log_" + std::to_string(i) + ".txt"));
        ofs << "dummy data";
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Ensure different timestamps
    }

    SECTION("Deletes oldest file when Count Limit (64) is reached") {
        REQUIRE(fs::exists(testDir / "log_0.txt"));
        
        // Initialize Sink
        // This should detect log_63 is the last one.
        // It will try to create log_64.
        // Since count is 64, adding 1 makes 65. It must delete log_0.
        FileSink sink(testDir);
        
        REQUIRE(fs::exists(testDir / "log_64.txt")); // New file created
        REQUIRE_FALSE(fs::exists(testDir / "log_0.txt")); // Oldest file deleted
        REQUIRE(fs::exists(testDir / "log_1.txt")); // Next oldest remains
    }

    fs::remove_all(testDir);
}

// NOTE: This test might take a few seconds to run because we have to write 50MB of data
TEST_CASE("FileSink Total Size Limit", "[FileSink]") {
    fs::path testDir = "./test_logs_size_limit";
    if (fs::exists(testDir)) fs::remove_all(testDir);
    fs::create_directories(testDir);

    // Goal: Trigger the 50MB limit with fewer than 64 files.
    // Strategy: Create 2 files manually that are 26MB each (Total 52MB).
    // The Sink should delete the oldest one on startup/rotation.
    
    // Create huge log_0.txt (26MB)
    {
        std::ofstream ofs0(testDir / "log_0.txt");
        std::string junk(1024 * 1024, 'A'); // 1MB string
        for(int i=0; i<26; ++i) ofs0 << junk;
        // timestamp needs to be older
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Create huge log_1.txt (26MB)
    {
        std::ofstream ofs1(testDir / "log_1.txt");
        std::string junk(1024 * 1024, 'B'); 
        for(int i=0; i<26; ++i) ofs1 << junk;
    }

    SECTION("Deletes files when Total Size > 50MB") {
        REQUIRE(fs::exists(testDir / "log_0.txt"));
        REQUIRE(fs::exists(testDir / "log_1.txt"));

        // When sink starts, it sees total size ~52MB > 50MB.
        // It should delete log_0 (oldest) to make room.
        FileSink sink(testDir);

        REQUIRE_FALSE(fs::exists(testDir / "log_0.txt")); // Deleted for space
        REQUIRE(fs::exists(testDir / "log_1.txt"));       // Kept
        REQUIRE(fs::exists(testDir / "log_2.txt"));       // Newly created
    }

    fs::remove_all(testDir);
}