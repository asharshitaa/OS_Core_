# Loader_in_OS

# SimpleLoader Implementation

To implement a SimpleLoader for loading an ELF 32-bit executable in plain-C without using any 
library APIs available for manipulating ELF files. 

## Overview
The SimpleLoader is a basic implementation designed to load and execute an ELF (Executable and Linkable Format) binary file in a Unix-like operating system. ELF files are the standard binary format for executables, object code, shared libraries, and core dumps. This implementation focuses on understanding the ELF file structure and manually loading and running an executable.

## ELF File Structure
ELF files have two main views: the linkable view and the execution view. The ELF header, located at the beginning of the file, serves as a roadmap for the entire file's organization. This header contains crucial information about the file, including the number of program headers and their locations.

### ELF Header (EHDR)
The ELF header contains several fields, but the key ones relevant to our loader are:

- **e_phnum**: The number of program headers in the executable file.
- **e_phoff**: The offset from the beginning of the file where the first program header is located.
- **e_entry**: The entry point virtual address where the system first transfers control at process startup.

### Program Header (PHRD)
The Program Header describes the segments of the program to be loaded into memory. Each segment has a type, such as `PT_LOAD`, indicating that the segment should be loaded into memory. The header also includes information about the segment's memory size, file size, and virtual address.

## SimpleLoader Working Steps
The following steps outline the process of loading and executing an ELF binary using SimpleLoader:

1. **Open and Read the ELF File**:
   - Use the `open` system call to obtain a file descriptor for the ELF binary.
   - Use the `read` system call to read the content of the binary into a dynamically allocated memory area using `malloc`.

2. **Iterate Through the Program Headers**:
   - Access the Program Header Table using the `e_phoff` value from the ELF header.
   - Iterate through each Program Header to find the segments of type `PT_LOAD`.

3. **Memory Allocation for Segments**:
   - For each `PT_LOAD` segment, allocate memory using the `mmap` function.
   - Copy the segment content into the allocated memory.

4. **Locate the Entrypoint**:
   - The entry point (`e_entry`) might not be at the beginning of the segment. Navigate through the segment to reach the virtual address specified by `e_entry`.

5. **Execute the Entrypoint**:
   - Once the entry point is located, cast the address to a function pointer that matches the signature of the `_start` method in the program (e.g., `fib.c`).
   - Call the `_start` method and print the returned value.

## Conclusion
The SimpleLoader demonstrates the core principles of loading and executing an ELF binary, providing a deeper understanding of the ELF file format and the basics of executable file management in Unix-like operating systems.

