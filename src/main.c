#include "defs.h"

int main(int argc, char *argv[]) {
    // Define grid parameters (size, max particles, and max iterations)
    uint32_t grid_size = 2;         // Example grid size (you can adjust)
    uint32_t max_particles = 2 * 2;   // Maximum number of particles to place
    uint32_t iteration_max = 256;   // Maximum number of iterations per particle

    // Initialize the grid
    grid_t *grid = grid_init(grid_size, max_particles, iteration_max);
    if (!grid) {
        fprintf(stderr, "[ERROR] Failed to initialize grid.\n");
        return EXIT_FAILURE;
    }

    // Start the DLA simulation
    printf("[DEBUG] Running DLA simulation with a grid of size %u and max particles %u\n", grid_size, max_particles);
    grid_run_simulation(grid);

    // Save the final grid state to a PNG image
    char *output_file = "dla_output.png";  // Output file name
    printf("[DEBUG] Saving grid to %s\n", output_file);
    save_grid(grid, output_file);

    // Free the grid
    grid_free(grid);

    printf("[DEBUG] Simulation complete. Image saved.\n");

    return EXIT_SUCCESS;
}
