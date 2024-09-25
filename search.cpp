#include "search.h"
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <cstring>

bool search(const string& currentDir, string& target)
{
    DIR* dir = opendir(currentDir.c_str());
    
    if(dir == nullptr)
    {
        perror("opendir error");
        return false;
    }

    struct dirent* entry;
    while((entry = readdir(dir)) != nullptr)
    {
        string name = entry->d_name;

        if(name == "." or name == "..")
            continue;

        if(name == target)
        {
            closedir(dir);
            return true;
        }

        if(entry->d_type == DT_DIR)
        {
            if(search(currentDir + "/" + name, target) == true)
            {
                closedir(dir);
                return true;
            }
        }
    }
    closedir(dir);
    return false;
}