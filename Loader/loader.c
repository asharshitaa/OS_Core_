#include "loader.h"


Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
char *heap_mem;
int file;


void loader_cleanup(){

  ehdr = NULL;
  phdr = NULL;  
  free(heap_mem);
  close(file);

}

void load_and_run_elf(char* exe) {   
    if (exe == NULL) {
        printf("Error: File path is NULL\n");
        exit(1);
    }

    // 1. Load entire binary content into the memory from the ELF file.   
    file = open(exe, O_RDONLY);
    if (file < 0) {
        perror("Error opening file");
        exit(1);
    }

    off_t file_size = lseek(file, 0, SEEK_END);

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

    phdr = (Elf32_Phdr *)(heap_mem + ehdr->e_phoff);
    void *addr_of_entry;

    // 2. Iterate through the PHDR table and find the section of PT_LOAD 
    //    type that contains the address of the entrypoint method in fib.c

    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {

            // 3. Allocate memory of the size "p_memsz" using mmap function 
            //    and then copy the segment content
            char *virtual_mem = mmap(NULL, phdr[i].p_memsz, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
            if (virtual_mem == MAP_FAILED) {
                perror("Memory mapping failed");
                free(heap_mem);
                close(file);
                exit(1);
            }

            memcpy(virtual_mem, heap_mem + phdr[i].p_offset, phdr[i].p_filesz);

            // 4. Navigate to the entrypoint address into the segment loaded in the memory in above step
            addr_of_entry = virtual_mem + (ehdr->e_entry - phdr[i].p_vaddr);
            if ((char *)addr_of_entry >= (char *)virtual_mem && (char *)addr_of_entry < (char *)virtual_mem + phdr[i].p_memsz) {
                break;
            }


        }
    }

    if (addr_of_entry) {

        // 5. Typecast the address to that of function pointer matching "_start" method in fib.c.
        int (*_start)(void) = (int (*)(void))addr_of_entry;
        
        // 6. Call the "_start" method and print the value returned from the "_start"
        int result = _start();  
        printf("User _start return value = %d\n", result);

    } else {
        printf("Entry Point Address is out of bounds.\n");
    }

}



int main(int argc, char** argv) 
{
    if(argc != 2) {
        printf("Usage: %s <ELF Executable> \n",argv[0]);
        exit(1);
    }

    // 1. carry out necessary checks on the input ELF file
    FILE *elf_file = fopen(argv[1], "rb");
    if (!elf_file){
        printf("Error: Unable to open ELF file.\n");
        exit(1);
    }
    fclose(elf_file);

    // 2. passing it to the loader for carrying out the loading/execution
    load_and_run_elf(argv[1]);
    
    // 3. invoke the cleanup routine inside the loader  
    loader_cleanup();
    return 0;
}
