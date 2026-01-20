#include "FileManager.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>
#include <cstdlib>

std::string FileManager::readFile(const std::string &filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        throw std::runtime_error("Could not open file: " + filepath);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void FileManager::writeFile(const std::string &filepath, const std::string &content)
{
    std::ofstream file(filepath);
    if (!file.is_open())
    {
        throw std::runtime_error("Could not create file: " + filepath);
    }

    file << content;
    file.close();

    if (file.fail())
    {
        throw std::runtime_error("Error writing to file: " + filepath);
    }
}

bool FileManager::fileExists(const std::string &filepath)
{
    std::ifstream file(filepath);
    return file.good();
}

std::string FileManager::ensureExtension(const std::string &filepath, const std::string &extension)
{
    std::string ext = extension;

    // Add dot if not present
    if (!ext.empty() && ext[0] != '.')
    {
        ext = "." + ext;
    }

    // Check if filepath already has this extension
    if (filepath.length() >= ext.length() &&
        filepath.compare(filepath.length() - ext.length(), ext.length(), ext) == 0)
    {
        return filepath; // Already has extension
    }

    return filepath + ext;
}

std::string FileManager::getExtension(const std::string &filepath)
{
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos && dotPos < filepath.length() - 1)
    {
        return filepath.substr(dotPos);
    }
    return "";
}

std::string FileManager::getDownloadsPath()
{
    std::filesystem::path downloadsPath;
#ifdef _WIN32
    const char *userProfile = std::getenv("USERPROFILE");
    if (userProfile)
    {
        downloadsPath = std::filesystem::path(userProfile) / "Downloads";
    }
#else
    const char *home = std::getenv("HOME");
    if (home)
    {
        downloadsPath = std::filesystem::path(home) / "Downloads";
    }
#endif

    if (!downloadsPath.empty() && std::filesystem::exists(downloadsPath))
    {
        return downloadsPath.string();
    }

    return std::filesystem::current_path().string();
}