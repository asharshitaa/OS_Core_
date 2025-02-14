#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "scheduler.h"


extern struct shared_mem *shared_mem;
int TSLICE; 
int NCPU;

void shell_loop();  
struct shared_mem *create_shared_mem(); 
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <NCPU> <TSLICE>\n", argv[0]);
        exit(1);
    }

    NCPU = atoi(argv[1]);
    TSLICE = atoi(argv[2]); 
    printf("Starting SimpleShell with NCPU = %d and TSLICE = %dms\n", NCPU, TSLICE);

    shared_mem = create_shared_mem(); 
    printf("Shared memory initialized.\n");

    if (fork() == 0) {
        scheduler_rr(); 
        exit(0);
    }

    shell_loop();  
}
