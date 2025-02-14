#ifndef SCHEDULER_H
#define SCHEDULER_H

#define MAX_JOBS 10000
extern int TSLICE; 

struct job {
    pid_t pid;               
    int is_active;           
    char command[256];      
};

struct shared_mem {
    int job_count;           
    int term_flag;   
    struct job job_queue[MAX_JOBS]; 
};


extern struct shared_mem *shared_mem; 


struct shared_mem *create_shared_mem(); 
void add_job(pid_t pid, const char *command);
void remove_job(int index);
void scheduler_rr();

#endif
