#include "defs.h"

int main() {
    srand(time(NULL));

    grid_t grid;
    uint32_t resolution = 8;
    uint32_t particle_num = 8 * 8;

    vector2_t particle_empty = {-1, -1};  // Initialize the "null" particle

    printf("[DEBUG] beginning grid constructor\n");
    grid_constructor(&grid, resolution, particle_num);

    printf("[DEBUG] beginning grid active\n");
    grid_active(&grid);

    printf("[DEBUG] beginnning grid to png\n");
    grid_to_png(&grid, "C:\\Users\\james\\source\\repos\\DLA-generation\\out\\fill-test.png");
    
    printf("[DEBUG] beginning grid deconstructor");
    grid_deconstructor(&grid);

    printf("[DEBUG] DONE");
    return EXIT_SUCCESS;  // Standard exit code for success
}