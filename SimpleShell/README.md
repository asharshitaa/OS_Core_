
# SimpleShell

## Overview
Implementation of Simple Shell in C that supports various functionalities such as executing commands, handling pipes, managing command history, and providing an interactive user interface. This shell is designed to demonstrate fundamental concepts of process management and inter-process communication in Unix-like operating systems.

## Features
- Command Execution: Execute simple commands and commands with arguments.
- Change Directory: Supports the `cd` command for changing the working directory.
- History Management: Maintains a history of executed commands along with their process IDs (PIDs) and timestamps for start and end.
- Pipe Support: Allows command execution with pipes (e.g., `command1 | command2`).
- Background Execution: Supports running processes in the background using the `&` symbol.
- Signal Handling: Catches SIGINT (Ctrl+C) to display command history before exiting.

## Functions defined
- read_user_input: Reads a line of input from the user.
- split_line: Splits a line into an array of arguments.
- cd_command: Changes the current directory.
- exit_from_shell: Exits the shell.
- add_to_history: Adds a command to the history with its PID and timestamp.
- show_shell_history: Displays the command history.
- mark_end_time: Marks the end time for a command in history.
- check_pipe: Checks if the input line contains a pipe.
- split_command_by_pipe: Splits the command line by pipe symbols.
- execute_pipes: Handles execution of piped commands.
- launch: Forks a process to execute a command.
- shell_loop: Main loop that drives the shell interaction.

## Limitations 
1. **Does not support input/output redirection (e.g., `command > file.txt`, `command < file.txt`)**:
   - Input/output redirection requires parsing symbols like `>`, `<`, and manipulating file descriptors, which has not been implemented in the current shell.

2. **Does not support command chaining with `&&` or `||`**:
   - Command chaining with `&&` and `||` requires advanced parsing and conditional execution logic that is not currently handled by the shell.

3. **Does not support subshell execution (e.g., `(command1; command2)`)**:
   - Subshells involve spawning a separate environment for grouped commands, which would require additional process creation logic that is outside the current scope.

4. **Does not handle signals for background jobs (e.g., suspending jobs with `Ctrl+Z`)**:
   - Signal handling for job control, such as suspending and resuming processes, requires integrating signal processing and process management features, which the shell does not currently support.