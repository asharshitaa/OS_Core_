# SimpleSmartLoader

## What It Does
SimpleSmartLoader is a tool that helps load and run ELF (Executable and Linkable Format) files. It opens an ELF file, loads it into memory, and runs it. Along the way, it handles things like page faults, which happen when the program tries to access parts of memory that haven't been loaded yet. This is useful for understanding how memory is managed at a low level.

## Key Features
1. **Loads ELF Files**: It reads the ELF file and prepares the necessary parts of it for execution.
2. **Handles Page Faults**: If a part of the program needs to be accessed but hasn’t been loaded yet, it maps the missing memory page instead of loading everything upfront.
3. **Tracks Memory Wastage**: It calculates how much space is wasted due to rounding when the program is loaded into memory.
4. **Shows Stats**: After running the program, it gives a summary of how many page faults happened, how many pages were allocated, and how much memory was wasted.

## How to Use
### Compiling the Code
To turn the code into a working program, run:
```bash
gcc -g -o SimpleSmartLoader loader.c -ldl
```

### Running the Program
Once compiled, you can run the loader with an ELF executable as input:
```bash
./SimpleSmartLoader <path_to_ELF_file>
```

For example:
```bash
./SimpleSmartLoader ./sum
```

### What You’ll See
- The address where each page fault happens.
- The entry point (starting point) of the program.
- A count of how many page faults occurred and how many pages were allocated.
- Information about memory waste due to page rounding, displayed in kilobytes.

### Functions Inside
1. **loader_cleanup**: Cleans up and prints memory waste (fragmentation) at the end.
2. **segfault_handler**: This handles page faults by loading missing pages into memory.
3. **load_and_run_elf**: Loads the ELF file, sets up the memory, and starts running the program.

### Error Handling
The program will check for common issues like:
- Problems reading the ELF file.
- Issues with memory mapping.
- Unsupported ELF formats.

If something goes wrong, it will print an error and stop.


link: 
