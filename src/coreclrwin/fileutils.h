#pragma once

#include <string>
#include <vector>

class FileUtils
{
public:
	static bool exists(const std::string &filename);
    static bool isDir(const std::string &fileOrDir);
    static std::string filename(const std::string &filePath);
    static std::string filenameWithoutExtension(const std::string &filePath);
    static std::string extension(const std::string &filepath);
    static std::string currentDir();
    static std::string parentDir(const std::string &file);
    static void normalizePaths(std::string &path);

    static bool isRelative(const std::string &path);

    static std::vector<std::string> list(
        const std::string &dir,
        const std::string &fileExtension);
    static std::vector<std::string> list(
        const std::string &dir,
        const std::vector<std::string> &fileExtensions = {});
};