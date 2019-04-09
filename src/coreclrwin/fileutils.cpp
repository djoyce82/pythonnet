#include "fileutils.h"

#include <algorithm>
#include <iostream>
#include <fstream>

#ifdef _WIN32
#include <filesystem>
using namespace std::tr2::sys;
namespace fs = std::tr2::sys;
#else
#include <experimental/filesystem>
using namespace std::experimental::filesystem;
namespace fs = std::experimental::filesystem;
#endif

#define UNUSED(x) [&x]{}()

std::string tostring(const fs::path &p)
{
#ifdef _WIN32
    const std::wstring w = p;
    const std::string res(w.begin(), w.end());
    return res;
#else
    return p;
#endif
}


bool FileUtils::exists(const std::string &fileName)
{
//    std::ifstream infile(fileName);
    //return infile.good();
    return fs::exists(path(fileName));
}

bool FileUtils::isDir(const std::string &fileOrDir)
{
    return exists(fileOrDir)
        && is_directory(path(fileOrDir.c_str()));
}

std::string FileUtils::filename(const std::string &filepath)
{
    return tostring(path(filepath).filename());
}

std::string FileUtils::filenameWithoutExtension(const std::string &filepath)
{
    return tostring(path(filepath).stem());
}

std::string FileUtils::extension(const std::string &filepath)
{
    return tostring(path(filepath).extension());
}

std::string FileUtils::currentDir()
{
#ifdef _WIN32
  return tostring(fs::current_path());
#else
     return fs::current_path();
#endif
}

std::string FileUtils::parentDir(const std::string &filename)
{
    path p(filename.c_str());

    return tostring(p.parent_path());
}

bool FileUtils::isRelative(const std::string &path)
{
#ifdef _WINDOWS
    return path.length() > 2 && path[1] != ':';
#else
    return path.length() > 1 && path[0] != '/';
#endif
}

void FileUtils::normalizePaths(std::string &paths)
{
#ifdef _WIN32
    std::replace(paths.begin(), paths.end(), '/', '\\');
#else
    UNUSED(paths);
#endif
}

std::vector<std::string> FileUtils::list(
        const std::string &dir,
        const std::string &fileExtension)
{
    if (fileExtension.length() > 1)
    {
        return FileUtils::list(dir, std::vector<std::string> { fileExtension });
    }
    else
    {
        return FileUtils::list(dir, std::vector<std::string> { });
    }
}

std::vector<std::string> FileUtils::list(
    const std::string &dir,
    const std::vector<std::string> &fileExtensions)
{
    path p(dir.c_str());
    std::vector<std::string> fileList;

    const auto filterFn = [fileExtensions](const std::string &name) -> bool
    {
        return fileExtensions.empty()
            || std::find(
                    fileExtensions.begin(),
                    fileExtensions.end(),
                    FileUtils::extension(name)) != fileExtensions.end();
    };

    for (auto i = directory_iterator(p); i != directory_iterator(); i++)
    {
        if (!isDir(tostring(i->path())) && filterFn(tostring(i->path().extension())))
        {
            fileList.push_back(tostring(i->path().filename()));
        }
    }

    return fileList;
}