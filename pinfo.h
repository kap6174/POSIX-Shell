#ifndef PINFO_H
#define PINFO_H

#include <iostream>
#include <string>
using namespace std;

string getProcStatus(int pid);
string getProcMemory(int pid);
string getProcExecutablePath(int pid);
void pinfo(int pid);

#endif // PINFO_H
