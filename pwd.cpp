#include <iostream>
#include "pwd.h"
#include <limits.h>
#include <unistd.h>


string rawPath()
{
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) 
    {
        return string(cwd);
    } 
    else 
    {
        perror("getcwd() error");
        return "";
    }
}

void PresentWorkingDirectory(string& baseDir)
{
    string currentDir = rawPath();

    if (currentDir.find(baseDir) == 0) 
    {
        currentDir = "/home" + currentDir.substr(baseDir.length());
    }

    cout << currentDir << endl;
}