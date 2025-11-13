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

    /**
     * Read entire file content as string
     * @param filepath Path to the file to read
     * @return File content as string
     * @throws std::runtime_error if file cannot be opened or read
     */
    static std::string readFile(const std::string &filepath);

    /**
     * Write content to file
     * @param filepath Path to the file to write
     * @param content Content to write to file
     * @throws std::runtime_error if file cannot be created or written
     */
    static void writeFile(const std::string &filepath, const std::string &content);

    /**
     * Check if file exists
     * @param filepath Path to check
     * @return true if file exists and is readable
     */
    static bool fileExists(const std::string &filepath);

    /**
     * Ensure file has given extension, add if missing
     * @param filepath Original filepath
     * @param extension Extension to ensure (with or without dot)
     * @return Filepath with extension
     */
    static std::string ensureExtension(const std::string &filepath, const std::string &extension);

    /**
     * Get file extension from filepath
     * @param filepath Path to extract extension from
     * @return Extension including dot (e.g. ".txt") or empty string if no extension
     */
    static std::string getExtension(const std::string &filepath);
};