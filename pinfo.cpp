#include "pinfo.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <vector>

using namespace std;

string getProcStatus(int pid) 
{
    string status;
    ifstream procStatus("/proc/" + to_string(pid) + "/status");
    if (procStatus.is_open()) 
    {
        string line;
        while (getline(procStatus, line)) 
        {
            if (line.find("State") != string::npos) 
            {
                status = line.substr(7, 1); // Extract the state character
                break;
            }
        }
        procStatus.close();
    }

    // Check if the process is in the foreground
    pid_t fg_pgid = tcgetpgrp(STDIN_FILENO); // Get the foreground process group ID
    pid_t pgid = getpgid(pid); // Get the process group ID of the given PID

    if (pgid == fg_pgid) 
    {
        status += "+"; // Append + if the process is in the foreground
    }

    return status;
}

string getProcMemory(int pid) 
{
    string memory;
    ifstream procStatus("/proc/" + to_string(pid) + "/status");
    if (procStatus.is_open()) 
    {
        string line;
        while (getline(procStatus, line)) 
        {
            if (line.find("VmSize") != string::npos) 
            {
                memory = line.substr(8, line.find("kB") - 8); // Extract virtual memory size
                break;
            }
        }
        procStatus.close();
    }
    return memory;
}

string getProcExecutablePath(int pid) 
{
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "/proc/%d/exe", pid);
    char resolved_path[1024];
    ssize_t len = readlink(buffer, resolved_path, sizeof(resolved_path) - 1);
    if (len != -1) 
    {
        resolved_path[len] = '\0';
        return string(resolved_path);
    }
    return "";
}

void pinfo(int pid) 
{
    cout << "pid -- " << pid << endl;
    cout << "Process Status -- {" << getProcStatus(pid) << "}" << endl;
    cout << "memory -- " << getProcMemory(pid) << " {Virtual Memory}" << endl;
    cout << "Executable Path -- " << getProcExecutablePath(pid) << endl;
}