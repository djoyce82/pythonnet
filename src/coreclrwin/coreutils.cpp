#include "coreutils.h"
#include "fileutils.h"
#ifdef _WIN32
#include "Windows.h"
#define FS_SEPARATOR "\\"
#define PATH_DELIMITER ";"
#define MAX_PATH 260
#endif
#include <vector>
#include <string>
#include <set>

bool GetEntrypointExecutableAbsolutePath(char **entrypointExecutable)
{
    char result[ MAX_PATH ];
    GetModuleFileName(NULL, result, MAX_PATH);

    *entrypointExecutable = strdup(result);

    return true;
}

bool GetDirectory(const char* absolutePath, char** directory)
{
    *directory = _strdup(absolutePath);
    if (*directory == NULL)
    {
        perror("Could not allocate buffer for path");
        return false;
    }

    size_t len = strlen(*directory);
    if((*directory)[len-1] == '/')
    {
        (*directory)[len-1] = '\0';
        return true;
    }

    return false;
}


#if _WIN32
// Win32 directory search for .dll files
// <Snippet7>
void BuildTpaList(const char* directory, const char* extension, std::string& tpaList)
{
    // This will add all files with a .dll extension to the TPA list.
    // This will include unmanaged assemblies (coreclr.dll, for example) that don't
    // belong on the TPA list. In a real host, only managed assemblies that the host
    // expects to load should be included. Having extra unmanaged assemblies doesn't
    // cause anything to fail, though, so this function just enumerates all dll's in
    // order to keep this sample concise.
    std::string searchPath(directory);
    searchPath.append(FS_SEPARATOR);
    searchPath.append("*");
    searchPath.append(extension);

    WIN32_FIND_DATAA findData;
    HANDLE fileHandle = FindFirstFileA(searchPath.c_str(), &findData);

    if (fileHandle != INVALID_HANDLE_VALUE)
    {
        do
        {
            // Append the assembly to the list
            tpaList.append(directory);
            tpaList.append(FS_SEPARATOR);
            tpaList.append(findData.cFileName);
            tpaList.append(PATH_DELIMITER);

            // Note that the CLR does not guarantee which assembly will be loaded if an assembly
            // is in the TPA list multiple times (perhaps from different paths or perhaps with different NI/NI.dll
            // extensions. Therefore, a real host should probably add items to the list in priority order and only
            // add a file if it's not already present on the list.
            //
            // For this simple sample, though, and because we're only loading TPA assemblies from a single path,
            // and have no native images, we can ignore that complication.
        }
        while (FindNextFileA(fileHandle, &findData));
        FindClose(fileHandle);
    }
}
// </Snippet7>
#elif LINUX
// POSIX directory search for .dll files
void BuildTpaList(const char* directory, const char* extension, std::string& tpaList)
{
    DIR* dir = opendir(directory);
    struct dirent* entry;
    int extLength = strlen(extension);

    while ((entry = readdir(dir)) != NULL)
    {
        // This simple sample doesn't check for symlinks
        std::string filename(entry->d_name);

        // Check if the file has the right extension
        int extPos = filename.length() - extLength;
        if (extPos <= 0 || filename.compare(extPos, extLength, extension) != 0)
        {
            continue;
        }

        // Append the assembly to the list
        tpaList.append(directory);
        tpaList.append(FS_SEPARATOR);
        tpaList.append(filename);
        tpaList.append(PATH_DELIMITER);

        // Note that the CLR does not guarantee which assembly will be loaded if an assembly
        // is in the TPA list multiple times (perhaps from different paths or perhaps with different NI/NI.dll
        // extensions. Therefore, a real host should probably add items to the list in priority order and only
        // add a file if it's not already present on the list.
        //
        // For this simple sample, though, and because we're only loading TPA assemblies from a single path,
        // and have no native images, we can ignore that complication.
    }
}
#endif

const char* GetEnvValueBoolean(const char* envVariable)
{
    const char* envValue = getenv(envVariable);
    if (envValue == NULL)
    {
        envValue = "0";
    }

    // CoreCLR expects strings "true" and "false" instead of "1" and "0".
    if (strcmp(envValue, "1") == 0)
    {
        return "true";
    }
    else
    {
        // Try again with lowercase
        char* value = strdup(envValue);
        if (value == NULL)
        {
            perror("Could not allocate buffer");
            return "false";
        }

        for (; *value; ++value) *value = tolower(*value);

        if (strcmp(value, "true") == 0)
        {
            return "true";
        }
    }

    return "false";
}
