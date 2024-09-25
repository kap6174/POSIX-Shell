#ifndef SHELL_H
#define SHELL_H

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>

using namespace std;

// Function declarations
long calculateTotalBlocks(const string& path, bool includeHidden);
string trim(const string& str);
vector<string> tokenizeByPipe(const string& input);
vector<string> tokenizeBySemicolon(const string& input);
vector<string> tokenize(const string& input);
string join(const vector<string>& tokens, const string& delimiter);
vector<string> tokenizeWithRedirection(const string& input, string& inputFile, string& outputFile, int& outputMode);
bool executeCommandWithRedirection(const vector<string>& tokens, const string& inputFile, const string& outputFile, int outputMode);
bool executePipeline(const vector<vector<string>>& commands, string& baseDir, string& currentDir, vector<string>& history, int& status);
bool executeCommand(vector<string>& tokens, string& baseDir, string& currentDir, const string& input, vector<string>& history, int& status);

#endif // SHELL_H
