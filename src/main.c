#include "defs.h"

int main() {
    srand(time(NULL));

    uint32_t grid_size = 2;
    uint32_t max_particles = 2 * 2;
    uint32_t iteration_max = 256;

    grid_t *grid = grid_init(grid_size, max_particles, iteration_max);

    grid_run_simulation(grid);

    save_grid(grid, "test1.png");

    grid_free(grid);

    return 0;
}