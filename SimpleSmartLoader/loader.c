#include <signal.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <elf.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
char *heap_mem;
int file;
int page_fault_count = 0;
int page_allocation_count = 0;
int fragmentation = 0;
off_t file_size;

void loader_cleanup() {
    free(heap_mem);
    close(file);

    fragmentation = 0;
    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            fragmentation += (4096 - (phdr[i].p_memsz % 4096)) % 4096;
        }
    }
    printf("Internal Fragmentation: %d KB\n", fragmentation / 1024);
}


void segfault_handler(int sig, siginfo_t *info, void *context) {
    void *wrong_addr = info->si_addr;
    printf("Page fault at address: 0x%p\n", wrong_addr);

    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            if (wrong_addr >= (void*)phdr[i].p_vaddr &&
                wrong_addr < (void*)(phdr[i].p_vaddr + phdr[i].p_memsz)) {
                
                size_t page_size = getpagesize();
                void *page_start = (void*)((uintptr_t)wrong_addr & ~(page_size - 1));
                
                char *virtual_mem = mmap(page_start, page_size, PROT_READ | PROT_WRITE | PROT_EXEC,
                                          MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
                if (virtual_mem == MAP_FAILED) {
                    perror("Memory mapping failed");
                    free(heap_mem);
                    close(file);
                    exit(1);
                }

                size_t offset_in_segment = (uintptr_t)page_start - phdr[i].p_vaddr;
                memcpy(virtual_mem, heap_mem + phdr[i].p_offset + offset_in_segment, page_size);
                page_fault_count++;
                page_allocation_count++;
                printf("Page faults: %d\n", page_fault_count);
                printf("Page allocations: %d\n", page_allocation_count);
                break;
            }
        }
    }
}


void load_and_run_elf(char* exe) {
    if (exe == NULL) {
        perror("Error: File path is NULL\n");
        exit(1);
    }

    file = open(exe, O_RDONLY);
    if (file < 0) {
        perror("Error opening file");
        exit(1);
    }

    file_size = lseek(file, 0, SEEK_END);
    lseek(file, 0, SEEK_SET);

    heap_mem = (char *)malloc(file_size);
    if (!heap_mem) {
        perror("Error: Memory allocation failed");
        close(file);
        exit(1);
    }

    ssize_t file_read = read(file, heap_mem, file_size);
    if (file_read < 0 || (size_t)file_read != file_size) {
        perror("Error: File read operation failed");
        free(heap_mem);
        close(file);
        exit(1);
    }

    ehdr = (Elf32_Ehdr *)heap_mem;
    if (ehdr->e_type != ET_EXEC) {
        printf("Unsupported ELF file\n");
        free(heap_mem);
        close(file);
        exit(1);
    }

    if (ehdr->e_phoff == 0) {
        printf("Error: Invalid program header offset (0x0)\n");
        free(heap_mem);
        close(file);
        exit(1);
    }

    phdr = (Elf32_Phdr *)(heap_mem + ehdr->e_phoff);


    void *addr_of_entry = NULL;

    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            char *virtual_mem = mmap(NULL, phdr[i].p_memsz, PROT_READ | PROT_WRITE | PROT_EXEC, 
                         MAP_PRIVATE, file, phdr[i].p_offset);


            if (virtual_mem == MAP_FAILED) {
                perror("Memory mapping failed");
                free(heap_mem);
                close(file);
                exit(1);
            }

            if (ehdr->e_entry >= phdr[i].p_vaddr && ehdr->e_entry < phdr[i].p_vaddr + phdr[i].p_memsz) {
                addr_of_entry = (void *)(ehdr->e_entry);
                printf("Entry point inside segm at: 0x%p\n", addr_of_entry);
                break;
            }
        }
    }

    if (addr_of_entry) {

        int (*_start)(void) = (int (*)(void))addr_of_entry;
        printf("User _start start value = %p\n", (void*)_start);
        int result = _start();
        printf("User _start return value = %d\n", result);

    } else {
        printf("Entry Point Address is out of bounds.\n");
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <ELF Executable> \n", argv[0]);
        exit(1);
    }

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = segfault_handler;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        perror("Error setting up SIGSEGV handler");
        exit(1);
    }
    FILE *elf_file = fopen(argv[1], "rb");
    if (!elf_file) {
        printf("Error: Unable to open ELF file.\n");
        exit(1);
    }
    fclose(elf_file);

    load_and_run_elf(argv[1]);

    printf("Total page faults: %d\n", page_fault_count);
    printf("Total page allocations: %d\n", page_allocation_count);
    printf("Total internal fragmentation: %d KB\n", fragmentation / 1024);

    loader_cleanup();
    return 0;
}
