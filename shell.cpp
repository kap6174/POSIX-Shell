#include "shell.h"
#include "cd.h"
#include "echo.h"
#include "pwd.h"
#include "history.h"
#include "search.h"
#include "ls.h"
#include "pinfo.h"
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>

using namespace std;

string previousDir;

long calculateTotalBlocks(const string& path, bool includeHidden) 
{
    DIR *dir;
    struct dirent *entry;
    struct stat fileStat;
    long totalBlocks = 0;

    if ((dir = opendir(path.c_str())) != NULL) 
    {
        while ((entry = readdir(dir)) != NULL) 
        {
            string fileName = entry->d_name;

            // Skip hidden files unless the all flag (-a) is set
            if (!includeHidden && fileName[0] == '.') 
            {
                continue;
            }

            string fullPath = path + "/" + fileName;

            if (stat(fullPath.c_str(), &fileStat) == 0) 
            {
                totalBlocks += fileStat.st_blocks;
            }
        }
        closedir(dir);
    } 
    else 
    {
        perror("opendir");
    }

    return totalBlocks / 2; // Convert to 512-byte blocks
}

string trim(const string& str) 
{
    size_t first = str.find_first_not_of(' ');
    if (first == string::npos) return ""; // No content
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

vector<string> tokenizeByPipe(const string& input) 
{
    vector<string> tokens;
    stringstream ss(input);
    string token;

    while (getline(ss, token, '|')) 
    {
        // Trim whitespace from the token and add to the vector
        tokens.push_back(trim(token));
    }
    
    return tokens;
}

// Function to tokenize input based on semicolon
vector<string> tokenizeBySemicolon(const string& input) 
{
    vector<string> tokens;
    stringstream ss(input);
    string token;

    while (getline(ss, token, ';')) 
    {
        // Trim whitespace from the token and add to the vector
        tokens.push_back(trim(token));
    }

    return tokens;
}

vector<string> tokenize(const string& input) 
{
    
    vector<string> tokens;
    char* str = new char[input.length() + 1];
    strcpy(str, input.c_str());

    char* token = strtok(str, " \t");

    while (token != NULL) 
    {
        tokens.push_back(string(token));
        token = strtok(NULL, " \t");
    }

    return tokens;
}

// Helper function to join command tokens into a single string
string join(const vector<string>& tokens, const string& delimiter) 
{
    string result;
    for (const auto& token : tokens) 
    {
        if (!result.empty()) 
        {
            result += delimiter;
        }
        result += token;
    }
    return result;
}

vector<string> tokenizeWithRedirection(const string& input, string& inputFile, string& outputFile, int& outputMode) 
{
    vector<string> tokens;
    stringstream ss(input);
    string token;

    inputFile = "";
    outputFile = "";
    outputMode = O_TRUNC; // Default output mode

    while (ss >> token) 
    {
        if (token == "<") 
        {
            ss >> inputFile; // Get the input file
        } 
        else if (token == ">") 
        {
            ss >> outputFile; // Get the output file
        } 
        else if (token == ">>") 
        {
            ss >> outputFile; // Get the output file
            outputMode = O_APPEND; // Change to append mode
        } 
        else 
        {
            tokens.push_back(trim(token)); // Add the token to the command list
        }
    }
    return tokens;
}

bool executeCommandWithRedirection(const vector<string>& tokens, const string& inputFile, const string& outputFile, int outputMode) 
{
    pid_t pid = fork();
    if (pid < 0) 
    {
        perror("Fork failed");
        return false;
    } 
    else if (pid == 0) 
    {
        // Child process

        // Handle input redirection
        if (!inputFile.empty()) 
        {
            int inputFd = open(inputFile.c_str(), O_RDONLY);
            if (inputFd < 0) 
            {
                cerr << "Error: Cannot open input file." << endl;
                exit(EXIT_FAILURE);
            }
            dup2(inputFd, STDIN_FILENO);
            close(inputFd);
        }

        // Handle output redirection
        if (!outputFile.empty()) 
        {
            int outputFd = open(outputFile.c_str(), O_WRONLY | O_CREAT | outputMode, 0644);
            if (outputFd < 0) 
            {
                cerr << "Error: Cannot open output file." << endl;
                exit(EXIT_FAILURE);
            }
            dup2(outputFd, STDOUT_FILENO);
            close(outputFd);
        }

        // Prepare arguments for execvp
        vector<char*> args;
        for (const auto& token : tokens) 
        {
            args.push_back(const_cast<char*>(token.c_str()));
        }
        args.push_back(nullptr); // Null-terminate the argument list

        // Execute the command
        if (execvp(args[0], args.data()) < 0) 
        {
            perror("Execvp failed");
            exit(EXIT_FAILURE);
        }
    }

    // Parent process waits for the child
    waitpid(pid, nullptr, 0);
    return true;
}

bool executePipeline(const vector<vector<string>>& commands, string& baseDir, string& currentDir, vector<string>& history, int& status) 
{
    int numCommands = commands.size();
    int pipefds[2 * numCommands]; // Pipes for communication between processes

    // Create pipes for all processes
    for (int i = 0; i < numCommands - 1; i++) 
    {
        if (pipe(pipefds + i * 2) < 0) 
        {
            perror("Pipe creation failed");
            return false;
        }
    }

    int pid;
    for (int i = 0; i < numCommands; i++) 
    {
        pid = fork();
        if (pid < 0) 
        {
            perror("Fork failed");
            return false;
        } 
        else if (pid == 0) 
        {
            // Child process

            // Check for I/O redirection
            string inputFile, outputFile;
            int outputMode;
            vector<string> commandTokens = commands[i]; // Get the command for this process

            // Tokenize the command to check for redirection
            commandTokens = tokenizeWithRedirection(trim(join(commandTokens, " ")), inputFile, outputFile, outputMode);

            // If not the first command, set input to previous pipe's read end
            if (i > 0) 
            {
                if (dup2(pipefds[(i - 1) * 2], 0) < 0) 
                {
                    perror("Dup2 input failed");
                    exit(1);
                }
            }

            // If not the last command, set output to next pipe's write end
            if (i < numCommands - 1) 
            {
                if (dup2(pipefds[i * 2 + 1], 1) < 0) 
                {
                    perror("Dup2 output failed");
                    exit(1);
                }
            }

            // Handle input redirection
            if (!inputFile.empty()) 
            {
                int inputFd = open(inputFile.c_str(), O_RDONLY);
                if (inputFd < 0) 
                {
                    cerr << "Error: Cannot open input file." << endl;
                    exit(EXIT_FAILURE);
                }
                dup2(inputFd, STDIN_FILENO);
                close(inputFd);
            }

            // Handle output redirection
            if (!outputFile.empty()) 
            {
                int outputFd = open(outputFile.c_str(), O_WRONLY | O_CREAT | outputMode, 0644);
                if (outputFd < 0) 
                {
                    cerr << "Error: Cannot open output file." << endl;
                    exit(EXIT_FAILURE);
                }
                dup2(outputFd, STDOUT_FILENO);
                close(outputFd);
            }

            // Close all pipe file descriptors in child
            for (int j = 0; j < 2 * numCommands; j++) 
            {
                close(pipefds[j]);
            }

            // Prepare arguments for execvp
            vector<char*> args;
            for (auto& token : commandTokens) 
            {
                args.push_back(const_cast<char*>(token.c_str()));
            }
            args.push_back(nullptr); // Null-terminate the argument list

            // Execute the command using execvp
            if (execvp(args[0], args.data()) < 0) 
            {
                perror("Execvp failed");
                exit(1);
            }
        }
    }

    // Parent process closes all pipe file descriptors
    for (int i = 0; i < 2 * numCommands; i++) 
    {
        close(pipefds[i]);
    }

    // Wait for all child processes
    for (int i = 0; i < numCommands; i++) 
    {
        wait(NULL);
    }

    return true;
}


// Function to execute commands
bool executeCommand(vector<string>& tokens, string& baseDir, string& currentDir, const string& input, vector<string>& history, int& status) 
{
    if (tokens.empty()) 
        return false;
    
    string inputFile, outputFile;
    int outputMode;

    // Tokenize the command and check for redirection
    auto commandTokens = tokenizeWithRedirection(input, inputFile, outputFile, outputMode);

    // If redirection is detected, execute with redirection
    if (!inputFile.empty() || !outputFile.empty()) 
    {
        return executeCommandWithRedirection(commandTokens, inputFile, outputFile, outputMode);
    }

    if (tokens[0] == "cd") 
    {
        string path;

        if (tokens.size() == 1) 
        {
            // No arguments provided, go to baseDir
            path = baseDir;
        } 
        else if (tokens.size() == 2) 
        {
            path = tokens[1];

            if (path == "~") 
            {
                path = baseDir;
            }
        } 
        else 
        {
            cerr << "cd: too many arguments" << endl;
            return false;
        }

        if (changeDirectory(path, previousDir)) 
        {
            return true;
        }

        return false;
    } 
    else if (tokens[0] == "echo") 
    {
        echoPrinting(input);
        return true;
    } 
    else if (tokens[0] == "pwd") 
    {
        PresentWorkingDirectory(baseDir);
        return true;
    } 
    else if (tokens[0] == "ls") 
    {
        bool all = false;
        bool longformat = false;
        string path = currentDir;

        vector<string> paths;
        for (long unsigned int i = 1; i < tokens.size(); i++) 
        {
            if (tokens[i] == "-a")
                all = true;
            else if (tokens[i] == "-l")
                longformat = true;
            else if (tokens[i] == "-la" || tokens[i] == "-al") 
            {
                all = true;
                longformat = true;
            }
            else if (tokens[i].front() != '-') 
            {
                path = tokens[i];
                if (path == "~") 
                {
                    path = baseDir;
                }
                paths.push_back(path);
            }
        }

        if (paths.size() > 1) 
        {
            for (auto path : paths) 
            {
                cout << path << ": " << endl;
                listing(path, all, longformat);
                cout << endl;
            }
            return true;
        } 
        else 
        {
            if (path == "~") 
            {
                path = baseDir;
            }

            if (longformat) 
            {
                // Calculate total block size before listing
                long totalBlocks = calculateTotalBlocks(path, all);
                cout << "total " << totalBlocks << endl;
            }

            return listing(path, all, longformat);
        }
    }


    else if (tokens[0] == "pinfo") 
    {
        // Check if a PID is provided
        int pid;
        if (tokens.size() == 2) 
        {
            pid = stoi(tokens[1]);
        } 
        else 
        {
            // Use the shell's own PID if no PID is provided
            pid = getpid();
        }

        // Call the pinfo function with the specified PID
        pinfo(pid);
        return true;
    }
    else if (tokens[0] == "search") 
    {
        if (tokens.size() != 2) 
        {
            cerr << "search: missing argument" << endl;
            return false;
        }
        string target = tokens[1];

        if(search(currentDir, target) == true)
            cout << "True" << endl;
        else
            cout << "False" << endl;
        
        return true;
    }
    else if (tokens[0] == "history") 
    {
        if(tokens.size() == 1)
        {
            displayHistory(history, 0);
        }
        else
        {
            // cout << "Size of array: " << history.size() << endl;
            displayHistory(history, stoi(tokens[1]));
        }
        return true;
    }
    else if (tokens[0] == "exit") 
    {
        status = 0;
        return true;
    }
    else 
    {
        bool isbg = false;
        if(tokens.back() == "&")
        {
            isbg = true;
            tokens.pop_back();
        }

        pid_t ppid = fork();
        if(ppid < 0)
        {
            perror("Fork Failed!");
            return false;
        }
        
        else if(ppid == 0)
        {
            vector<char*> args;
            for (auto& token : tokens) 
            {
                args.push_back(const_cast<char*>(token.c_str()));
            }
            args.push_back(nullptr);

            if (execvp(args[0], args.data()) == -1) 
            {
                perror("Error executing command");
                exit(EXIT_FAILURE); // Exit the child process with an error status
            }
        }

        else
        {
            if(isbg == true)
            {
                cout << "Background process PID: " << ppid << endl;
                if(setpgid(ppid, ppid) == -1)
                {
                    perror("Error setting group id");
                    return false;
                }
            }
            else
            {


                int commstatus;
                waitpid(ppid, &commstatus, 0); // Wait for the child process to complete

                if (WIFEXITED(commstatus)) 
                {
                    if (WEXITSTATUS(commstatus) != 0) 
                    {
                        cerr << "Command failed with exit status " << WEXITSTATUS(commstatus) << endl;
                    }
                } 
                else 
                {
                    cerr << "Command did not exit normally" << endl;
                }
                
            }
                    
        }

    }
    return true;
}


