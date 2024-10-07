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

// CMD Arguments:
// 1 grid size
// 2 particles per row
// 3 iteration max
// 4 output file name
// 5 grid output on/off
// 6 grid output filename

typedef struct {
    uint32_t grid_size;
    uint32_t max_particles;
    uint32_t iteration_max;
    bool output_grid;
    char* filename;
    char* grid_filename;
} arguments_t;

arguments_t args_init(int argc, char **argv) {
    // Check if the number of arguments is correct
    if (argc < 5) {
        printf("Usage: %s <grid size> <particle number> <iteration max> <output file name> [grid output on/off] [grid out filename]\n", argv[0]);
        exit(1);
    }

    arguments_t arguments;
    arguments.grid_size = 0;
    arguments.max_particles = 0;
    arguments.iteration_max = 0;
    arguments.output_grid = false;
    arguments.filename = NULL;
    arguments.grid_filename = NULL;

    // Parse required arguments
    arguments.grid_size = (uint32_t)atoi(argv[1]);
    arguments.max_particles = (uint32_t)atoi(argv[2]) * arguments.grid_size;
    arguments.iteration_max = (uint32_t)atoi(argv[3]);

    // Allocate memory for the output filename and copy it
    arguments.filename = (char*)malloc(strlen(argv[4]) + 1);
    strcpy(arguments.filename, argv[4]);

    // Parse the optional grid output arguments
    if (argc > 5 && strcmp(argv[5], "on") == 0) {
        arguments.output_grid = true;
    }

    if (arguments.output_grid && argc > 6) {
        arguments.grid_filename = (char*)malloc(strlen(argv[6]) + 1);
        strcpy(arguments.grid_filename, argv[6]);
    }

    return arguments;
}

void free_args(arguments_t *arguments) {
    if (arguments->filename) {
        free(arguments->filename);
    }
    if (arguments->grid_filename) {
        free(arguments->grid_filename);
    }
}

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
    arguments_t args = args_init(argc, argv);

    // Initialize grid and run the simulation
    grid_t *grid = grid_init(args.grid_size, args.max_particles, args.iteration_max);

    grid_run_simulation(grid);

    // Save grid as PNG
    save_grid_png(grid, args.filename);

    // If grid output is enabled, save the grid to a text file
    if (args.output_grid) {
        save_grid_txt(grid, args.grid_filename);
    }

    // Free resources
    grid_free(grid);
    free_args(&args);

    return 0;
}
