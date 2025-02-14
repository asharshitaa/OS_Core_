#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include "scheduler.h"  
struct shared_mem *shared_mem;

struct shared_mem *create_shared_mem() {
    int shm_fd = shm_open("/simple_scheduler_shm", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Failed to create shared memory");
        exit(1);
    }

    if (ftruncate(shm_fd, sizeof(struct shared_mem)) == -1) {
        perror("Failed to set size for shared memory");
        exit(1);
    }

    shared_mem = mmap(0, sizeof(struct shared_mem), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED) {
        perror("Failed to map shared memory");
        exit(1);
    }

    shared_mem->job_count = 0;
    shared_mem->term_flag = 0;
    return shared_mem;
}

void scheduler_rr() {
    while (1) {  
        if (shared_mem->term_flag) {
            printf("Scheduler terminating.\n");
            break;  
        }

        if (shared_mem->job_count == 0) {
            usleep(10000); 
            continue;
        }

        for (int i = 0; i < shared_mem->job_count; i++) {
            struct job *job = &shared_mem->job_queue[i];

            if (job->is_active) {
                
                kill(job->pid, SIGCONT);  
                usleep(TSLICE * 1000);                   
                kill(job->pid, SIGSTOP);  
                int status;
                pid_t result = waitpid(job->pid, &status, WUNTRACED); 
             
                if (result == -1) {
                    if (errno == ECHILD) {
                    
                        remove_job(i); 
                        i--;  
                    } else {
                        perror("waitpid failed");
                    }
                    continue;  
                }
                if (WIFEXITED(status) || WIFSIGNALED(status)) {
                    printf("Scheduler: Job PID %d finished, removing from queue.\n", job->pid);
                    remove_job(i);
                    i--;  
                } else {                    
                    printf("Job PID %d is still running.\n", job->pid);
                }
            }
        }
    }
}

void add_job(pid_t pid, const char *command) {
    if (shared_mem->job_count < MAX_JOBS) {
        struct job *job = &shared_mem->job_queue[shared_mem->job_count++];
        job->pid = pid;
        job->is_active = 1;
        strcpy(job->command, command);
    } else {
        printf("Scheduler job queue full\n");
    }
}

void remove_job(int index) {
    if (index < 0 || index >= shared_mem->job_count) return;
    for (int j = index; j < shared_mem->job_count - 1; j++) {
        shared_mem->job_queue[j] = shared_mem->job_queue[j + 1];
    }
    shared_mem->job_count--;
}
