#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>
#include "scheduler.h"  

extern struct shared_mem *shared_mem;

#define MAX_LINE 1024  
#define MAX_ARGS 64    
#define MAX_HISTORY 500

int pid_history[MAX_HISTORY];
char *history[MAX_HISTORY];
int history_count = 0;
time_t time_history[MAX_HISTORY][2];



char *read_user_input() {
    char *line = malloc(MAX_LINE * sizeof(char));
    if (fgets(line, MAX_LINE, stdin) == NULL) {
        perror("fgets");
        exit(1);
    }
    return line;
}

char **split_line(char *line) {
    char **args = malloc(MAX_ARGS * sizeof(char*));
    char *arg;
    int pos = 0;

    arg = strtok(line, " \t\r\n");
    while (arg != NULL) {
        args[pos] = arg;
        pos++;
        arg = strtok(NULL, " \t\r\n");
    }
    args[pos] = NULL;  
    return args;
}

int cd_command(char **args){
    if (args[1] == NULL){
        perror("no argument given");
    } else {
        if (chdir(args[1]) != 0){
            perror("cd");
        }
    }
    return 1;
}

int exit_from_shell(char **args){
    return 0;
}

void add_to_history(char *line, int pid){
    if (history_count < MAX_HISTORY){
        history[history_count] = strdup(line);
        pid_history[history_count] = pid;
        time(&time_history[history_count][0]);  
        history_count++;
    }
}

int show_shell_history(){
    printf("--------------------------------------------------------\n");
    for (int i = 0; i < history_count; i++) {
        printf("Command: %s\n", history[i]);
        printf("PID: %d\n", pid_history[i]);
        printf("Start Time: %s", ctime(&time_history[i][0]));  
        printf("End Time: %s", ctime(&time_history[i][1]));    
        printf("-------------------------------\n");
    }
    return 1;
}

void mark_end_time(int index) {
    if (index >= 0 && index < history_count) {
        time(&time_history[index][1]);  
    }
}

bool check_pipe(char *line) {
    return strchr(line, '|') != NULL;
}

char **split_command_by_pipe(char *line, int *num_pipes) {
    int pos = 0;
    char **commands = malloc(MAX_ARGS * sizeof(char *));
    char *command = strtok(line, "|");
    
    while (command != NULL) {
        commands[pos] = strdup(command);
        pos++;
        command = strtok(NULL, "|");
    }
    commands[pos] = NULL;
    *num_pipes = pos;
    return commands;
}

int execute_pipes(char **commands, int num) {
    int pipe_fds[2 * (num - 1)];
    pid_t pid;

    for (int i = 0; i < num - 1; i++) {
        if (pipe(pipe_fds + 2 * i) < 0) {
            perror("pipe");
            exit(1);
        }
    }

    for (int i = 0; i < num; i++) {
        pid = fork();
        if (pid == 0) {
            if (i > 0) {
                dup2(pipe_fds[(i - 1) * 2], STDIN_FILENO);
            }
            if (i < num - 1) {
                dup2(pipe_fds[i * 2 + 1], STDOUT_FILENO);
            }
            for (int j = 0; j < 2 * (num - 1); j++) {
                close(pipe_fds[j]);
            }
            char **args = split_line(commands[i]);
            if (execvp(args[0], args) < 0) {
                perror("execvp");
                exit(EXIT_FAILURE);
            }          
        } else if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < 2 * (num - 1); i++) {
        close(pipe_fds[i]);
    }
    for (int i = 0; i < num; i++) {
        wait(NULL);
    }
    return pid;
}

void handle_sigint(int sig) {
    printf("\nDisplaying command history:\n");
    show_shell_history();  
    exit(0);
}

int launch(char **args) {
    pid_t pid;
    int bg = 0;
    char command[MAX_LINE] = "";  
    int num = 0;
    
    if (strcmp(args[0], "submit") == 0){
            num = 1;
    }   
    for (int i = num; args[i] != NULL; i++) {
        if (strcmp(args[i], "&") == 0) {
            bg = 1;
            args[i] = NULL;
            break;
        }
        strcat(command, args[i]);
        strcat(command, " ");
    }
    pid = fork();
    if (pid == 0) {  
        if (execvp(args[num], &args[num]) == -1) { 
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }
    } else if (pid > 0) {  
        printf("Submitting job to scheduler: PID = %d, Command = %s\n", pid, command);
        add_job(pid, command);

        if (!bg) {
            int status;
            waitpid(pid, &status, 0);  
        }
    } else {
        perror("fork failed");
    }
    return pid;
}

void shell_loop() {
    char *line;
    char **args;
    int status = 1;

    do {
        printf("Harshita's Shell:~$ ");
        line = read_user_input();
        if (check_pipe(line)) {
            int num_pipes;
            char* linee = strdup(line);
            char **commands = split_command_by_pipe(line, &num_pipes);
            int pid = execute_pipes(commands, num_pipes);
            add_to_history(linee, pid); 
            mark_end_time(history_count - 1);
            free(commands);
        } else {
            char *linee = strdup(line);
            args = split_line(line);
            if (args != NULL && args[0] != NULL){
                if (strcmp(args[0], "cd") == 0) {
                    cd_command(args);
                } else if (strcmp(args[0], "history") == 0) {
                    show_shell_history();
                } else if (strcmp(args[0], "exit") == 0) {
                    break;  
                } else {
                    int pid = launch(args);
                    add_to_history(linee, pid); 
                    mark_end_time(history_count - 1); 
                }
                free(args);
            }
            else {
                fprintf(stderr, "No command entered or error in splitting line.\n");
            }
        }

        free(line);
    } while (status);
}
