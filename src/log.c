#include "defs.h"


void log_constructor(log_t *log, const char *filename) {
    // Allocate memory for the filename (plus one for the null terminator)
    log->filename = (char*)malloc(strlen(filename) + 1);
    if (log->filename) {
        // Copy the filename into log->filename
        strcpy(log->filename, filename);
    }
}

void log_write(log_t *log, const char *format, ...) {
    // Open the file for appending ("a" to avoid overwriting)
    FILE *fptr = fopen(log->filename, "a");
    if (!fptr) {
        perror("Error opening file");
        return;
    }

    // Initialize the va_list and handle the variable arguments
    va_list args;
    va_start(args, format);

    // Write the formatted string to the file
    vfprintf(fptr, format, args);

    // Clean up
    va_end(args);
    fclose(fptr);
}

void log_deconstructor(log_t *log) {
    if (log->filename) {
        free(log->filename);  // Free the filename memory
    }
    free(log);  // Free the log structure itself
}