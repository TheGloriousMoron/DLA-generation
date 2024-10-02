#include "defs.h"

void grid_constructor(grid_t *grid, uint32_t size, uint8_t particle_num) {
    grid->grid_size = size;
    grid->particle_num = particle_num;
    grid->particle_empty = Vector2((int32_t*){-1, -1});

    // allocate memory for the particle array
    printf("[DEBUG] creating particle array columns");
    grid->particle_array = malloc(sizeof(vector2_t) * grid->grid_size); // allocate memory for each column of the grid

    for (int x = 0; x < grid->grid_size; x++) {
        printf("[DEBUG] creating particle array row for column %d", x);
        grid->particle_array[x] = malloc(sizeof(vector2_t) * grid->grid_size);
    }

    // set all of the particles to particle_empty
    for (int x = 0; x < grid->grid_size; x++) {
        for (int y = 0; y < grid->grid_size; y++) {
            printf("[DEBUG] setting particle array %d, %d to particle empty", x, y);
            grid->particle_array[x][y] = grid->particle_empty;
        }
    }

    // set the origin particle
    vector2_t origin_particle;
    origin_particle = Vector2((int32_t*){rand() % grid->grid_size, rand() % grid->grid_size});
    printf("[DEBUG] origin particle created at %d, %d", origin_particle.x, origin_particle.y);
    grid->particle_array[origin_particle.x][origin_particle.y] = origin_particle;
    printf("[DEBUG] origin particle set");

    printf("[DEBUG] grid succesfully constructed");
}

void grid_active(grid_t *grid) {
    uint8_t particles_done = 0;

    for (int i = 0; i < grid->particle_num; i++) {
        bool is_done = false;
        bool is_stuck = true;
        vector2_t p = Vector2((int32_t*){rand() % grid->grid_size, rand() % grid->grid_size});
        // check if p is inside an already existing particle
        while (is_stuck) {
            if (Vector2_is_equal(grid->particle_array[p.x][p.y], grid->particle_empty)) {
                is_stuck = false;
            } else {
                p = Vector2((int32_t*){rand() % grid->grid_size, rand() % grid->grid_size});
            }
        }

        while (!is_done) {
            uint8_t move_dir = rand() % 4; // generate a number between 0 and 3
            switch (move_dir) {
                case 0:
                    if (p.x + 1 < grid->grid_size) { // use less than becuase the array is indexed at 0 and the grid size is not
                        // check for other particles
                        if (!Vector2_is_equal(grid->particle_array[p.x+1][p.y], grid->particle_empty)) {
                            is_done = true;
                            grid->particle_array[p.x + 1][p.y] = Vector2((int32_t*){p.x + 1, p.y});
                            printf("[DEBUG] particle %d set at: %d, %d", i, p.x + 1, p.y );
                        } else {
                            is_done = false;
                            p.x += 1;
                        }
                    } else {
                        is_done = false;
                    }
                case 1:
                    if (p.x - 1 >= 0) {
                        if (!Vector2_is_equal(grid->particle_array[p.x-1][p.y], grid->particle_empty)) {
                            is_done = true;
                            grid->particle_array[p.x - 1][p.y] = Vector2((int32_t*){p.x - 1, p.y});
                            printf("[DEBUG] particle %d set at: %d, %d", i, p.x - 1, p.y );
                        } else {
                            is_done = false;
                            p.x += -1;
                        }
                    } else {
                        is_done = false;
                    }
                case 2:
                    if (p.y + 1 < grid->grid_size) {
                        if (!Vector2_is_equal(grid->particle_array[p.x][p.y + 1], grid->particle_empty)) {
                            is_done = true;
                            grid->particle_array[p.x][p.y + 1] = Vector2((int32_t*){p.x, p.y + 1});
                            printf("[DEBUG] particle %d set at: %d, %d", i, p.x, p.y + 1 );
                        } else {
                            is_done = false;
                            p.y += 1;
                        }
                    } else {
                        is_done = false;
                    }
                case 3:
                    if (p.y - 1 >= 0) {
                        if (!Vector2_is_equal(grid->particle_array[p.x][p.y-1], grid->particle_empty)) {
                            is_done = true;
                            grid->particle_array[p.x - 1][p.y] = Vector2((int32_t*){p.x, p.y-1});
                            printf("[DEBUG] particle %d set at: %d, %d", i, p.x, p.y -1 );
                        } else {
                            is_done = false;
                            p.y += -1;
                        }
                    } else {
                        is_done = false;
                    }
                default:
                    is_done = false;
            }
            // switch done
        }
    }
}

void grid_deconstructor(grid_t grid) {
    // free each row in the array
    for (int x = 0; x  < grid.grid_size; x++) {
        printf("[DEBUG] freeing particle row %d", x);
        free(grid.particle_array[x]);
        printf("[DEBUG] freed particle row %d", x);
    }

    printf("[DEBUG] freeing particle array columns");
    free(grid.particle_array);
    printf("[DEBUG] freed particle array");

    printf("[DEBUG] freeing grid");
    free(&grid);
    printf("[DEBUG] grid freed");
}