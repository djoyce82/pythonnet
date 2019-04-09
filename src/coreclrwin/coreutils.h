#ifndef CORE_RUN_COMMON_H
#define CORE_RUN_COMMON_H

#include <string>

// Get directory of the specified path.
// Return true in case of a success, false otherwise.
bool GetDirectory(const char* absolutePath, char** directory);
void BuildTpaList(const char* directory, const char* extension, std::string& tpaList);
const char* GetEnvValueBoolean(const char* envVariable);
bool GetEntrypointExecutableAbsolutePath(char **entrypointExecutable);


#endif