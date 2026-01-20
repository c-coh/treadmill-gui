#pragma once
#include <string>
#include <vector>

/**
 * Static utility class for file operations
 * Provides common file I/O functionality without requiring instantiation
 */
class FileManager
{
public:
    // Delete constructor to prevent instantiation
    FileManager() = delete;

    static std::string readFile(const std::string &filepath);
    static void writeFile(const std::string &filepath, const std::string &content);
    static bool fileExists(const std::string &filepath);

    static std::string ensureExtension(const std::string &filepath, const std::string &extension);
    static std::string getExtension(const std::string &filepath);
    static std::string getDownloadsPath();
};