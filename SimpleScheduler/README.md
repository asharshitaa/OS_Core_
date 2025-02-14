
# SimpleShell Scheduler Project

## Overview

This project implements a simple shell that uses a round-robin scheduler to manage multiple jobs (processes). It utilizes shared memory for communication between the shell and the scheduler.

## Files

### 1. `main.c`
- **Purpose**: The main entry point of the shell program.
- **Key Functions**:
  - `int main(int argc, char *argv[])`: Initializes the shared memory and starts the shell loop and the scheduler.

### 2. `MyShell.c`
- **Purpose**: Contains the shell logic to interact with the user.
- **Key Functions**:
  - `void shell_loop()`: Runs the main loop of the shell, handling user commands.
  - `int exit_from_shell(char **args)`: Exits the shell when the user types `exit`.
  - `void handle_sigint(int sig)`: Handles the interrupt signal (Ctrl+C) to stop the shell gracefully.
  - `void launch(char **args)`: Launches a job (process) based on user input.

### 3. `scheduler.c`
- **Purpose**: Manages job scheduling using a round-robin algorithm.
- **Key Functions**:
  - `struct SharedMemory *create_shared_memory()`: Creates and initializes shared memory for job data.
  - `void scheduler_round_robin()`: Implements the round-robin scheduling algorithm for jobs.
  - `void add_job_to_scheduler(pid_t pid, const char *command)`: Adds a new job to the scheduler's job queue.
  - `void remove_job_from_queue(int index)`: Removes a finished job from the queue.

### 4. `scheduler.h`
- **Purpose**: Header file that contains structure definitions and function declarations for the scheduler.
- **Key Elements**:
  - `struct Job`: Defines the job structure with process ID, status, and command.
  - `struct SharedMemory`: Defines the shared memory structure for job management.
  - `extern int TSLICE;`: Declares the TSLICE variable used for time slicing in scheduling.
