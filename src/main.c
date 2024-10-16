#include "defs.h"
#include "grid.h"

const char *help_msg = "arguments:\n"
"-h print this message.\n"
"-r <uint32_t> the resolution of the grid and image.\n"
"-p <float> the percentage of the grid that will have particles as a float normalized between 0.0 and 1.0.\n"
"-o <string> the name of the output file with header.\n"
"-d output the grid as a text file.\n"
"-g <string> the name plus header of the file with the output of the grid.\n";

// CMD Arguments:
// -h, --help           Print this comments
// -s, --gridsize       grid size
// -p, --particlecount     particles per row
// -o, --output         output file name
// -d, --debug          grid output on/off
// -g, --gridout        grid output filename

int main(int argc, char **argv) {
    get_paths();

    // Seed random number generator
    srand((unsigned int)time(NULL));

    // Initialize arguments from command-line input
    arguments_t* args = NULL;
    args = args_init(argc, argv);

    if (args->help) {
        printf("%s",help_msg);
    }

    if (!args->help) {   
        // Initialize grid and run the simulation
        grid_t *grid = (grid_t*)malloc(sizeof(grid_t));
        grid_alloc(grid, args->grid_size, args->particle_coverage);
        if (args->load_grid) {
            char *grid_data = NULL;
            load_grid_file(args->load_grid_name, grid_data);
            uint32_t size, p_count;
            vector_t *p_pos = NULL;
            convert_grid_data(grid_data, &size, &p_count, p_pos);
            grid_init(grid, p_count, p_pos, (float*){0}, 0);
        } else {
            vector_t *p = (vector_t*){&(vector_t){grid->size / 2, grid->size / 2}};
            grid_init(grid, p, 1, (vector_t*){NULL}, (float*){0}, 0);
        }
        
        // run simulation
        grid_simulate_particles(grid);

        rgba_t **data = grid_get_simulation_data(grid);

        // Save grid as PNG
        save_grid_png(grid, data, args->out_name);

        // If grid output is enabled, save the grid to a text file
        if (args->debug) {
            save_grid_txt(grid, data, args->grid_out_name);
        }

        for(int i = 0; i < grid->size; i++) {
            free(data[i]);
        }
        free(data);
        grid_free(grid);
    }

    free_args(args);

    return 0;
}
