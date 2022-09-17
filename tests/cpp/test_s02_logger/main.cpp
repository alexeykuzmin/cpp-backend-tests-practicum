#include "my_logger.h"

#include <string_view>
#include <thread>
#include <filesystem>
#include <cassert>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std::literals;


bool CompareFiles(const std::filesystem::path& file_path, const std::filesystem::path& ref_file_path) {
    if (file_path.filename() != ref_file_path.filename()) {
        std::cout << "file_path: " << file_path << std::endl;
        std::cout << "ref_file_path: " << ref_file_path << std::endl;
        std::cout << "Filename error! " << std::endl;
        return false;
    }
    std::ifstream fs(file_path, std::ifstream::binary|std::ifstream::ate);
    std::ifstream ref_fs(ref_file_path, std::ifstream::binary|std::ifstream::ate);

    if (fs.fail() || ref_fs.fail()) {
        std::cout << "file_path: " << file_path << std::endl;
        std::cout << "ref_file_path: " << ref_file_path << std::endl;
        std::cout << "Error reading file! " << std::endl;
        return false;
    }

    if (fs.tellg() != ref_fs.tellg()) {
        std::cout << "file_path: " << file_path << std::endl;
        std::cout << "ref_file_path: " << ref_file_path << std::endl;
        std::cout << "Error: file size mismatch! " << std::endl;
        return false;
    }

    fs.seekg(0, std::ifstream::beg);
    ref_fs.seekg(0, std::ifstream::beg);
    if (!std::equal(
            std::istreambuf_iterator<char>(fs.rdbuf()),
            std::istreambuf_iterator<char>(),
            std::istreambuf_iterator<char>(ref_fs.rdbuf()))) {
        std::cout << "file_path: " << file_path << std::endl;
        std::cout << "ref_file_path: " << ref_file_path << std::endl;
        std::cout << "Error: file content mismatch! " << std::endl;
        return false;
    }
    return true;
}

std::filesystem::path GoUp(const std::filesystem::path& base, int steps) {
    if (steps <= 0) {
        return base;
    } else {
        return GoUp(base.parent_path(), steps - 1);
    }
}

int main() {
    const std::filesystem::path logs_path("/var/log");
    const std::filesystem::path reference_path = GoUp(std::filesystem::current_path(), 5) / std::filesystem::path("cpp-backend-tests-practicum/tests/cpp/test_s02_logger/reference");

    Logger::GetInstance().SetTimestamp(std::chrono::system_clock::time_point{1000000s});
    LOG("Hello "sv, "world "s, 123);
    LOG(1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 
        1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 
        1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0);    
    
    Logger::GetInstance().SetTimestamp(std::chrono::system_clock::time_point{10000000s});
    LOG("Brilliant logger.", " ", "I Love it");

    static const int attempts = 100000;
    for(int i = 0; i < attempts; ++i) {
        Logger::GetInstance().SetTimestamp(std::chrono::system_clock::time_point(std::chrono::seconds(10000000 + i * 100)));
        LOG("Logging attempt ", i, ". ", "I Love it");
    }

    for (const auto& entry : std::filesystem::directory_iterator{reference_path}) {
        std::filesystem::path filename = entry.path().filename();
        if(!CompareFiles(logs_path / filename, entry.path())) {
            std::cout << "Check failed!" << std::endl;
            return -1;
        }
    }
    
    std::cout << "Success!" << std::endl;
}
