#include <iostream>
#include <unistd.h>
#include <limits.h>
#include <string>
#include <pwd.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <signal.h>
#include "shell.h"
#include "history.h"

using namespace std;

// Global variable to track the foreground process ID
pid_t foregroundPid = -1;

// Function to get the current working directory
string getCurrentDirectory() 
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

// Function to get the username
string getUserName() 
{
    struct passwd *pw;

    if ((pw = getpwuid(geteuid())) != nullptr) 
    {
        return string(pw->pw_name);
    } 
    else 
    {
        perror("getpwuid() error");
        return "";
    }
}

// Function to get the system name
string getSystemName() 
{
    struct utsname buffer;

    if (uname(&buffer) != 0) {
        perror("uname");
        return "";
    }

    return string(buffer.sysname);
}

// Function to format and display the shell prompt
void displayPrompt(const string& baseDir, string& currentDir) 
{
    string relativeDir = currentDir;

    if (currentDir.find(baseDir) == 0) 
    {
        relativeDir = "~" + currentDir.substr(baseDir.length());
    }

    cout << getUserName() << "@" << getSystemName() << ":" << relativeDir << "> ";
}

// Signal handler for SIGTSTP (CTRL-Z)
void handleSigTSTP(int sig) 
{
    if (foregroundPid > 0) 
    {
        kill(foregroundPid, SIGSTOP); // Stop the foreground process
        cout << "\nProcess " << foregroundPid << " stopped (CTRL-Z)" << endl;
    }
}

// Signal handler for SIGINT (CTRL-C)
void handleSigINT(int sig) 
{
    if (foregroundPid > 0) 
    {
        kill(foregroundPid, SIGINT); // Interrupt the foreground process
        cout << "\nProcess " << foregroundPid << " interrupted (CTRL-C)" << endl;
    }
}

// Function to set up signal handlers
void setupSignalHandlers() 
{
    struct sigaction saTSTP, saINT;

    saTSTP.sa_handler = handleSigTSTP;
    sigemptyset(&saTSTP.sa_mask);
    saTSTP.sa_flags = 0;
    sigaction(SIGTSTP, &saTSTP, NULL);

    saINT.sa_handler = handleSigINT;
    sigemptyset(&saINT.sa_mask);
    saINT.sa_flags = 0;
    sigaction(SIGINT, &saINT, NULL);
}

int main() 
{
    string baseDir = getCurrentDirectory(); // The directory where the shell was started
    vector<string> history;
    loadHistory(history);

    int status = 1;

    setupSignalHandlers(); // Set up signal handlers

    while (status) 
    {
        string currentDir = getCurrentDirectory();
        // Display the prompt
        displayPrompt(baseDir, currentDir);

        // Read user input
        string input;
        getline(cin, input);

        if(cin.eof())
        {
            cout << endl;
            break;
        }

        // Tokenize by semicolon first
        vector<string> commandList = tokenizeBySemicolon(input);

        addCommToHistory(input, history);

        // Process each command in the command list (semicolon-separated)
        for (long unsigned int i = 0; i < commandList.size(); i++) 
        {
            string command = commandList[i];

            // Tokenize by pipe for piped commands
            vector<string> pipeCommands = tokenizeByPipe(command);

            if (pipeCommands.size() > 1) 
            {
                // If the command contains pipes, handle the pipeline
                vector<vector<string>> commandTokens;

                // Tokenize each individual command in the pipeline
                for (auto& cmd : pipeCommands) 
                {
                    commandTokens.push_back(tokenize(cmd));
                }

                // Execute the piped commands
                if (!executePipeline(commandTokens, baseDir, currentDir, history, status)) 
                {
                    cerr << "Failed to execute pipeline" << endl;
                }
            } 
            else 
            {
                // No pipe, treat it as a single command
                vector<string> singleCommandTokens = tokenize(command);

                // Check for built-in commands like `exit` before forking
                if (singleCommandTokens[0] == "exit") 
                {
                    status = 0;
                    break;
                }
                // Handle `cd` directly in the parent process
                if (singleCommandTokens[0] == "cd") 
                {
                    // Execute `cd` in the parent process
                    if (!executeCommand(singleCommandTokens, baseDir, currentDir, command, history, status)) 
                    {
                        cerr << "Failed to execute cd command" << endl;
                    }
                }

                else 
                {
                    // Set the foreground process ID before executing
                    foregroundPid = fork();
                    if (foregroundPid == 0) 
                    {
                        // In child process
                        if (singleCommandTokens[0] != "cd") 
                        {
                            if (!executeCommand(singleCommandTokens, baseDir, currentDir, command, history, status)) 
                            {
                                cerr << "Failed to execute command" << endl;
                            }
                        }
                        exit(0); // Exit child process after execution
                    } 
                    else 
                    {
                        // In parent process, wait for the foreground process
                        int commstatus;
                        waitpid(foregroundPid, &commstatus, 0);
                        foregroundPid = -1; // Reset foreground process ID
                    }
                }

                if (status == 0) 
                {
                    break; // Exit the shell loop if `exit` was called
                }
            }
        
        }

    }
    return 0;
    
}
    