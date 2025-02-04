<!-- To view the README.md in proper format open preview-->
# POSH - POSIX Shell Implementation

---

This project involves implementing a custom shell program in C++ that mimics some functionalities of a Unix-like shell. The shell supports executing commands, handling input and output redirection, piping commands, and managing foreground and background processes. It also includes support for built-in commands such as `cd`, `pwd`, `exit` and many more. 

### Key Features

- **Command Execution**: Executes system commands and built-in shell commands.
- **Redirection**: Supports input and output redirection (`>`, `>>`, `<`).
- **Pipelines**: Handles multiple commands connected by pipes (`|`).
- **Signal Handling**: Manages signals for stopping (CTRL-Z) and interrupting (CTRL-C) processes.
- **Built-in Commands**: Includes multiple built-in commands like `cd` for changing directories, `pwd` for displaying the current directory, and `exit` for exiting the shell and a lot more.
- **Error Handling**: Provides error messages for invalid commands or arguments.

The shell reads user input, processes commands, and executes them while handling errors and providing appropriate feedback.

## Project Structure

- `main.cpp`: The entry point of the program, handles the main loop, command parsing, and execution.
- `shell.cpp` / `shell.h`: Contains the implementation and declarations for shell-related functionalities.
- `cd.cpp` / `cd.h`: Implements the `cd` command for changing directories.
- `echo.cpp` / `echo.h`: Implements the `echo` command for printing messages to the terminal.
- `pwd.cpp` / `pwd.h`: Implements the `pwd` command to display the current working directory.
- `history.cpp` / `history.h` / `history.txt`: Manages the command history.
- `search.cpp` / `search.h`: Contains functions for searching files.
- `ls.cpp` / `ls.h`: Implements the `ls` command to list directory contents.
- `pinfo.cpp` / `pinfo.h`: Implements the `pinfo` command to display process information.
- `makefile`: To facilitate the building and management of the project

## Building and Running the Shell Program

This project includes a C++ shell program, and a Makefile is provided to facilitate the building and management of the project. Below are the steps to compile and run the program.

### Building the Program

1. **Compile the Program**:
   To compile the entire project, simply run the following command in your terminal:
   ```bash
   make
   ```
   This command will use the Makefile to compile all the source files and link them into an executable named `cc`.

2. **Clean Up**:
   To remove all object files and the compiled executable, use:
   ```bash
   make clean
   ```
   This command deletes all intermediate object files (`*.o`) and the executable (`cc`), allowing you to start fresh.

### Running the Program

1. **Execute the Shell**:
   Once the compilation is complete, you can run the shell program using:
   ```bash
   ./cc
   ```
   This will start the shell and present you with a command prompt where you can enter and execute commands.

## cd.cpp

### Description
The `cd.cpp` file contains the implementation of the `changeDirectory` function, which handles directory changes within the shell. This function supports various flags and arguments for changing directories, including:
- `"."` (current directory)
- `".."` (parent directory)
- `"-"` (previous directory)
- `"~"` (home directory)

### Functionality
- **Change to Specified Directory**: The function attempts to change the current working directory to the one specified by the `path` argument.
- **Error Handling**: If changing the directory fails, an error message is printed.

### Example Usage
```bash
junaid-ahmed@Linux:~> cd /path/to/directory
junaid-ahmed@Linux:/path/to/directory> cd -
/path/to/previous/directory
junaid-ahmed@Linux:/path/to/previous/directory> cd ~
junaid-ahmed@Linux:~> cd ..
junaid-ahmed@Linux:/home> cd .
```
### Notes 
* If no argument is provided to `cd`, the function changes the directory to the user's home directory. 
* If an invalid number of arguments is provided, or if the specified directory does not exist, the function will print an appropriate error message.

## echo.cpp

### Description
The `echo.cpp` file contains the implementation of the `echoPrinting` function, which processes and prints the input string to the standard output. It handles quoted strings and multiple tokens, ensuring proper formatting.

### Functionality
- **Tokenization**: The `echotokenize` function splits the input string into tokens based on spaces and quotes. It recognizes quoted strings and preserves them as single tokens.
- **Printing**: The `echoPrinting` function processes these tokens and prints them to the standard output. It removes unnecessary quotes and handles various formatting aspects.
- **Handling Quotes**: It supports quotes, treating quoted text as a single token and removing the quotes during output.

### Example Usage
```bash
junaid-ahmed@Linux:~> echo "Hello, World!"
Hello, World!

junaid-ahmed@Linux:~> echo "This is a test with 'a' single quote inside it."
This is a test with 'a' single quote inside it.

junaid-ahmed@Linux:~> echo Hello        World
Hello        World

junaid-ahmed@Linux:~> echo "No quotes here"
No quotes here
```
### Notes 
* Double quotes `"` are used to include spaces in the output. 
* Single quotes `'` are ignored when given without quotes otherwise retained in the output. 
* The function ensures that any tokens or spaces within quotes are printed as-is, without tampering.

## pwd.cpp

### Description
The `pwd.cpp` file provides functionality to display the present working directory. It formats the path to show relative to the user's home directory when appropriate.

### Functionality
- **Retrieve Current Directory**: Uses `getcwd` to obtain the current working directory.
- **Format Output**: Adjusts the path to show as relative to the user's home directory, if applicable.

### Example Usage
```bash
junaid-ahmed@Linux:~/dir> pwd
/home/dir
```

## ls.cpp

### Description
The `ls.cpp` file implements the `ls` command with support for the `-a` (all) and `-l` (long format) flags and other variations of these two as well. It allows listing files and directories with various options.

### Functionality
- **Flags Supported**:
  - `-a`: Shows all files, including hidden ones.
  - `-l`: Displays detailed file information (permissions, number of links, owner, group, size, modification time).
- **Path Handling**:
  - Lists contents of the specified directory or current directory if no path is provided.
  - Supports relative paths (`.` and `..`) and home directory (`~`).


### Example Usage
```bash
junaid-ahmed@Linux:~> ls -l test_dir
junaid-ahmed@Linux:~> ls -la /path/to/dir1 /path/to/dir2
```

## pinfo.cpp

### Description
The `pinfo.cpp` file implements the `pinfo` command to display process-related information of a given process ID (PID). It does not use `popen()` and retrieves information directly from the `/proc` filesystem.

### Functionality
- **`pinfo`**: Displays the following information for the current shell process:
  - PID
  - Process Status (e.g., Running, Sleeping, Zombie, Stopped)
  - Memory Usage (Virtual Memory)
  - Executable Path

- **`pinfo <pid>`**: Displays the same information for the specified PID.

### Example Usage

- **`pinfo`**: Displays information for the current shell process.
```bash
junaid-ahmed@Linux:~> pinfo
pid -- 231
Process Status -- {R+}
memory -- 67854 {Virtual Memory}
Executable Path -- /home/user/a.out
junaid-ahmed@Linux:~> pinfo 7777
pid -- 7777
Process Status -- {S}
memory -- 123456 {Virtual Memory}
Executable Path -- /usr/bin/gcc
```
## search.cpp

### Description
The `search.cpp` file implements the `search` command to recursively search for a specified file or folder under the current directory. It returns `True` if the file or folder exists, and `False` otherwise.

### Functionality
- **`search <filename>`**: Recursively searches for the specified file or folder in the current directory and its subdirectories.

### Example Usage

```bash
junaid-ahmed@Linux:~> search xyz.txt
True
junaid-ahmed@Linux:~> search abc.txt
False
```
## history.cpp

### Description
The `history.cpp` file implements the `history` command, which tracks and displays the last executed commands in a shell session. It stores up to 20 commands, overwriting the oldest when exceeding this limit. The command can display the last 10 commands by default or a specified number of recent commands.

### Functionality
- **`history`**: Displays the most recent 10 commands.
- **`history <num>`**: Displays the latest `<num>` commands, where `<num>` is an integer up to 20.

### Examples

- **`history`**: Displays the last 10 commands stored.

```bash
junaid-ahmed@Linux:~> history 3
pwd
cd
history
```
## shell.cpp

### Description
`shell.cpp` is a comprehensive implementation of a custom shell in C++, supporting various Unix-like shell functionalities, including command execution, built-in commands, input/output redirection, and piping. The shell can handle both foreground and background processes, supports command history, and allows for complex command chaining using pipes and redirection.

### Core Functionalities

1. **Command Execution**: 
   - The shell supports executing built-in commands (`cd`, `echo`, `pwd`, `ls`, `history`, `search`, `pinfo`) as well as rest of the unix commands via `execvp`.
   - Background execution is supported by appending `&` to a command.
   
2. **Pipes (`|`)**:
   - The shell supports piped commands where the output of one command is passed as input to the next.
   - For example, `ls -l | grep ".cpp" | wc -l` will count the number of `.cpp` files in the current directory.

3. **Input/Output Redirection**:
   - Supports redirecting input using `<` and output using `>` or `>>`.
   - For example, `cat file.txt > output.txt` will write the contents of `file.txt` to `output.txt`, overwriting it, while `>>` will append the content.

4. **Command Chaining**:
   - Commands can be chained together using semicolons (`;`), allowing multiple commands to be executed sequentially.
   - Example: `ls; pwd; echo "Done"` will execute the `ls`, `pwd`, and `echo` commands in sequence.


### Examples

1. **Simple Command Execution**:
   - Command: `ls -l`
   - Executes `ls` with the `-l` option to display detailed file information.

2. **Piped Command**:
   - Command: `ls -l | grep ".cpp"`
   - Lists files in the directory and filters them to show only `.cpp` files.
3. **Command Chaining**:
   - Command: `mkdir new_folder; cd new_folder; touch file.txt`
   - Creates a new directory, moves into it, and creates a new file.

4. **Redirection**:
   - Command: `grep "main" main.cpp > results.txt`
   - Searches for the term "main" in `main.cpp` and writes the results to `results.txt`.

5. **Pipeline with Redirection**:
   - Command: `ls | grep .txt > out.txt`
   - Searches for `.txt` files in the directory and stores them in out.txt

6. **Pipeline with Redirection with Semi-Colon**:
   - Command: `ls | grep .txt > out.txt; cat < in.txt | wc -l > lines.txt`
   - Searches for `.txt` files in the directory and stores them in out.txt, and then another pipeline with redirection command that passes the content of in.txt to wc -l which counts the number of lines in the file and stores them in lines.txt.

7. **Background Process**:
   - Command: `gedit &`
   - Runs `gedit` in the background, allowing the shell to accept new commands immediately.

## main.cpp

The `main.cpp` file serves as the entry point for the shell program. It handles critical tasks such as displaying the shell prompt, reading user input, and calling functions from `shell.cpp` for command execution. Below is a brief overview:

- **Displaying Prompt**: The `displayPrompt()` function formats and displays the shell prompt using the current directory, username, and system name.
- **Handling User Input**: User input is read and tokenized into individual commands or pipelines using functions from `shell.cpp`.
- **Command Execution**: Commands are processed and executed by calling appropriate functions from `shell.cpp`.

### Signal Handling

A crucial feature of `main.cpp` is its signal handling capability, allowing the shell to respond to user interrupts (`CTRL+Z`) and stop signals (`CTRL+C`):

- **SIGTSTP (CTRL+Z)**: Stops the current foreground process. The `handleSigTSTP()` function sends a `SIGSTOP` signal to the process, pausing its execution and allowing it to be resumed later.


- **SIGINT (CTRL+C)**: Interrupts the current foreground process. When `CTRL+C` is pressed, the `handleSigINT()` function sends a `SIGINT` signal to the foreground process, terminating it. This prevents the shell from being inadvertently terminated by user interrupts.

- **(CTRL+D)**: It should log you out of your shell and close it.

# Conclusion

The shell program we've implemented brings together various components to create a functional, responsive, and user-friendly command-line interface. At its core, `main.cpp` coordinates the flow of operations, handling tasks such as displaying prompts, managing user input, and controlling command execution.

Key functionalities like command parsing, history management, and built-in command execution are handled efficiently within `shell.cpp`, which is invoked seamlessly by `main.cpp`. The thoughtful integration of these features allows the shell to support essential commands, manage process pipelines, and maintain a command history, making it a powerful tool for interacting with the operating system.

In summary, this shell implementation not only provides a solid foundation for executing commands but also enhances the user experience. It reflects a well-rounded approach to building a shell that is both functional and robust, offering users a practical and effective means to interact with their system.




