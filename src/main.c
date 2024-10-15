#include "defs.h"

char OUTPATH[MAX_PATH_LENGTH];
char LOGPATH[MAX_PATH_LENGTH];

#ifdef _WIN32
void win_get_outpath(char *outpath) {
    char exe_path[MAX_PATH_LENGTH];
    GetModuleFileName(NULL, exe_path, MAX_PATH_LENGTH);

    char *pos = strstr(exe_path, "build");
    if (pos != NULL) {
        strcpy(pos, "out\\");
    } else {
        strcat(exe_path, "\\out\\");
    }

    strcpy(outpath, exe_path);
}
#else
void unix_get_outpath(char *outpath) {
    char exe_path[MAX_PATH_LENGTH];
    ssize_t count = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (count != -1) {
        exe_path[count] = '\0'; // Null-terminate the string
    } else {
        perror("readlink");
        exit(1);
    }

    char *pos = strstr(exe_path, "build");
    if (pos != NULL) {
        strcpy(pos, "out/");
    } else {
        strcat(exe_path, "/out/");
    }

    strcpy(outpath, exe_path);
}
#endif

const char *help_msg = "arguments:\n"
"-h print this message.\n"
"-s <uint32_t> the size of the grid.\n"
"-p <uint32_t> the number of particles for each row.\n"
"-o <string> the name of the output file with header.\n"
"-d output the grid as a text file.\n"
"-g the name plus header of the file with the output of the grid.\n";

// CMD Arguments:
// -h, --h              Print this comments
// -s, --gridsize       grid size
// -p, --particlecount     particles per row
// -o, --output         output file name
// -d, --debug          grid output on/off
// -g, --gridout        grid output filename



int main(int argc, char **argv) {
    #ifdef _WIN32
        win_get_outpath(OUTPATH);
        win_get_outpath(LOGPATH);
    #else
        unix_get_outpath(OUTPATH);
        unix_get_outpath(LOGPATH);
    #endif

    // Seed random number generator
    srand((unsigned int)time(NULL));

    // Initialize arguments from command-line input
    arguments_t* args = NULL;
    args = args_init(argc, argv);

    if (args->help) {
        printf("%s",help_msg);
    }

    // Initialize grid and run the simulation
    grid_t *grid = grid_init(args);
    
    if (!args->help) {    
        grid_run_simulation(grid);

        // Save grid as PNG
        save_grid_png(grid, args->out_name);
    }

    // If grid output is enabled, save the grid to a text file
    if (args->debug) {
        save_grid_txt(grid, args->grid_out_name);
    }

    // Free resources
    grid_free(grid);
    free_args(args);

    return 0;
}
