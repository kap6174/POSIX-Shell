#include "history.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>


void saveHistory(vector<string> &history)
{
    ofstream file("history.txt");
    for (long unsigned int i = 0; i < history.size(); i++)
    {
        file << history[i] << endl;
    }
    file.close();
}
void addCommToHistory(string &command, vector<string> &history)
{
    if(history.size() >= 20)
    {
        history.erase(history.begin());
    }
    history.push_back(command);
    saveHistory(history);
}
void loadHistory(vector<string> &history)
{
    ifstream file("history.txt");
    string command;
    while (getline(file, command))
    {
        history.push_back(command);
    }
    file.close();
}
void displayHistory(vector<string> &history, int count)
{
    if(count == 0)
        count = 10;
    for(long unsigned int i = history.size() - count; i < history.size(); i++)
    {
        cout << history[i] << endl;
    }
}