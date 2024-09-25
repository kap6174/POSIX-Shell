#ifndef HISTORY_H
#define HISTORY_H

#include <string>
#include <vector>

using namespace std;

// Function declarations
void saveHistory(vector<string> &history);
void addCommToHistory(string &commmand, vector<string> &history);
void loadHistory(vector<string> &history);
void displayHistory(vector<string> &history, int count);


#endif // HISTORY_H
