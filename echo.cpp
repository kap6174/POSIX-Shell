#include "echo.h"
#include <unistd.h>
#include <iostream>
#include <vector>

using namespace std;

vector<string> echotokenize(const string& input)
{
    vector<string> tokens;
    bool inQuotes = false;
    string token;

    for (char c : input) 
    {
        if (c == '"') 
        {
            inQuotes = !inQuotes;
            token += c;
        } 
        else if (isspace(c) && inQuotes == false) 
        {
            if (!token.empty()) 
            {
                tokens.push_back(token + " ");
                token.clear();
            }
        } 
        else 
        {
            token += c;
        }
    }

    if (token.empty() == false) 
    {
        tokens.push_back(token);
    }

    return tokens;
}

void echoPrinting(const string& input)
{
    vector<string> echotokens = echotokenize(input);
    for(long unsigned int i = 1; i < echotokens.size(); i++)
    {
        string str;
        bool doubleQuotes = false;
        for(long unsigned int j = 0; j < echotokens[i].size(); j++)
        {
            if(echotokens[i][j] == '"')
            {
                doubleQuotes = true;
                continue;
            }
            if(doubleQuotes == false and echotokens[i][j] == '\'')
                continue;
            else
                str += echotokens[i][j];
        }
        cout << str;
    }
    cout << endl;
}