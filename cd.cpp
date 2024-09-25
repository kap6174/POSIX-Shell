#include "cd.h"
#include <unistd.h>
#include <iostream>
#include <limits.h>

using namespace std;

bool changeDirectory(const string& path, string& previousDir)
{
    static string currentDir;

    // Save the current directory to previousDir before changing
    char temp[PATH_MAX];
    if (getcwd(temp, sizeof(temp)) != nullptr) 
    {
        currentDir = string(temp);
    } 
    else 
    {
        perror("Error getting current directory");
        return false;
    }

    if (path == "-") 
    {
        if (previousDir.empty()) 
        {
            cerr << "cd: OLDPWD not set" << endl;
            return false;
        }

        if (chdir(previousDir.c_str()) != 0) 
        {
            perror("Error Changing Directory");
            return false;
        }
        cout << previousDir << endl;
    } 
    else 
    {
        if (chdir(path.c_str()) != 0) 
        {
            perror("Error Changing Directory");
            return false;
        }
    }

    previousDir = currentDir;
    return true;
}