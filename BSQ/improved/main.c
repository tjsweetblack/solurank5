#include "bsq.h"

/*
 * process_file - Open and process a file
 * 
 * Returns: 0 on success, -1 on error
 */
static int process_file(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
        return -1;
    
    int result = process_map(file);
    fclose(file);
    
    return result;
}

/*
 * main - Entry point
 * 
 * Handles three cases:
 * 1. No arguments: read from stdin
 * 2. Single argument: process that file
 * 3. Multiple arguments: process each file, separate output with newlines
 * 
 * On any error, prints "map error" to stderr and continues with next file.
 */
int main(int argc, char **argv)
{
    if (argc == 1) {
        /* No arguments: read from stdin */
        if (process_map(stdin) == -1)
            fprintf(stderr, "map error\n");
    }
    else {
        /* Process each file argument */
        for (int i = 1; i < argc; i++) {
            if (process_file(argv[i]) == -1)
                fprintf(stderr, "map error\n");
            
            /* Print newline between outputs (but not after last) */
            if (i < argc - 1)
                printf("\n");
        }
    }
    
    return 0;
}
