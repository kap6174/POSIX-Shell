#include "ls.h"
#include <iostream>
#include <dirent.h>
#include <ctime>
#include <sys/stat.h>
#include <grp.h>
#include <iomanip>
#include <pwd.h>

using namespace std;

bool listing(string& path, bool all, bool longformat)
{
    DIR *dir = opendir(path.c_str());

    if(!dir)
    {
        perror("opendir error");
        return false;
    }
    struct dirent* entry;
    while((entry = readdir(dir)) != nullptr)
    {
        string name = entry->d_name;

        if(all == false and name[0] == '.')
            continue;
        
        if(longformat == true)
        {
            struct stat info;
            string fullPath = path + "/" + name;
            if(stat(fullPath.c_str(), &info) == -1)
            {
                perror("stat error");
                closedir(dir);
                return false;
            }
            if (S_ISDIR(info.st_mode))
                cout << "d";
            else
                cout << "-";

            if (info.st_mode & S_IRUSR)
                cout << "r";
            else
                cout << "-";

            if (info.st_mode & S_IWUSR)
                cout << "w";
            else
                cout << "-";

            if (info.st_mode & S_IXUSR)
                cout << "x";
            else
                cout << "-";

            if (info.st_mode & S_IRGRP)
                cout << "r";
            else
                cout << "-";

            if (info.st_mode & S_IWGRP)
                cout << "w";
            else
                cout << "-";

            if (info.st_mode & S_IXGRP)
                cout << "x";
            else
                cout << "-";

            if (info.st_mode & S_IROTH)
                cout << "r";
            else
                cout << "-";

            if (info.st_mode & S_IWOTH)
                cout << "w";
            else
                cout << "-";

            if (info.st_mode & S_IXOTH)
                cout << "x";
            else
                cout << "-";

            cout << " " << info.st_nlink;

            struct passwd *pw = getpwuid(info.st_uid);
            struct group *gr = getgrgid(info.st_gid);
            if (pw)
                cout << " " << pw->pw_name;
            else
                cout << " ";

            if (gr)
                cout << " " << gr->gr_name;
            else
                cout << " ";

            cout << " " << setw(8) << info.st_size;

            char timebuffer[80];
            strftime(timebuffer, sizeof(timebuffer), "%b %d %H:%M", localtime(&info.st_mtime));
            cout << " " << timebuffer;

            cout << " " << name << endl;
        }
        else
            cout << name << endl;
    }
    closedir(dir);
    return true;

}