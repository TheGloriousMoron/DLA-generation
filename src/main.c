#include "defs.h"

int main() {
    grid_t grid;
    uint32_t resolution = 64;
    uint8_t particle_num = 20;

    particle_t particle_empty = {-1, -1};  // Initialize the "null" particle

    dla_init(&grid, &particle_empty, (uint32_t)resolution);
    generation_loop(&grid, particle_num, resolution);

    char *filename = "test.png";
    save_raw_rgba(&grid, filename);

    grid_deconstruct(&grid);

    return EXIT_SUCCESS;  // Standard exit code for success
}