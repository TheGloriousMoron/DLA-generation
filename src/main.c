#include "defs.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

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
