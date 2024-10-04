#include "defs.h"

// Initialize the grid for the DLA simulation
grid_t* grid_init(uint32_t size, uint32_t max_particles, uint32_t iteration_max) {
    grid_t *grid = (grid_t*)malloc(sizeof(grid_t));
    if (!grid) {
        perror("[ERROR] Failed to allocate memory for grid");
        exit(EXIT_FAILURE);
    }

    grid->size = size;
    grid->max_particles = max_particles;
    grid->iteration_max = iteration_max;
    grid->current_particles = 0;
    grid->fill_state = 1;
    grid->adjecent_state = 2;
    grid->empty_state = 0;

    grid->array = (int8_t**)malloc(size * sizeof(int8_t*));
    if (!grid->array) {
        perror("[ERROR] Failed to allocate memory for grid rows");
        free(grid);
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < size; i++) {
        grid->array[i] = (int8_t*)malloc(size * sizeof(int8_t));
        if (!grid->array[i]) {
            perror("[ERROR] Failed to allocate memory for grid columns");
            for (uint32_t j = 0; j < i; j++) {
                free(grid->array[j]);
            }
            free(grid->array);
            free(grid);
            exit(EXIT_FAILURE);
        }
        memset(grid->array[i], grid->empty_state, size * sizeof(int8_t)); // Initialize with empty state
    }

    return grid;
}

// Function to check if a particle is inside the grid
bool check_particle_inside(grid_t *grid, int32_t *pos) {
    return pos[0] >= 0 && pos[0] < grid->size && pos[1] >= 0 && pos[1] < grid->size;
}

// Function to check if a particle is adjacent to an existing one
bool check_particle_adjecent(grid_t *grid, int32_t *pos) {
    // Check all 4 adjacent positions
    int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    for (int i = 0; i < 4; i++) {
        int new_r = pos[0] + directions[i][0];
        int new_c = pos[1] + directions[i][1];
        if (check_particle_inside(grid, (int32_t[]){new_r, new_c}) && grid->array[new_r][new_c] == grid->fill_state) {
            return true;
        }
    }
    return false;
}

// Function to check if a position is clear (not filled)
bool check_position_clear(grid_t *grid, int32_t *pos) {
    return grid->array[pos[0]][pos[1]] == grid->empty_state;
}

// Set a particle at a position
void set_particle(grid_t *grid, int32_t *pos) {
    grid->array[pos[0]][pos[1]] = grid->fill_state;
    grid->current_particles++;
}

// Function to run the DLA simulation
void grid_run_simulation(grid_t *grid) {
    printf("[DEBUG] Starting simulation\n");

    uint32_t pos[2] = {rand() % grid->size, rand() % grid->size};
    while (!check_particle_inside(grid, (int32_t*)pos)) {
        pos[0] = rand() % grid->size;
        pos[1] = rand() % grid->size;
    }

    set_particle(grid, (int32_t*)pos);  // Place the first particle

    while (grid->current_particles < grid->max_particles) {
        uint32_t particle_iterations = 0;

        // Randomly choose a starting position for the new particle
        pos[0] = rand() % grid->size;
        pos[1] = rand() % grid->size;

        // Ensure the new particle is in a valid position
        while (!check_particle_inside(grid, (int32_t*)pos) || !check_position_clear(grid, (int32_t*)pos)) {
            pos[0] = rand() % grid->size;
            pos[1] = rand() % grid->size;
        }

        while (particle_iterations < grid->iteration_max) {
            bool valid_move = false;
            uint8_t move_dir;
            int32_t new_pos[2] = {pos[0], pos[1]};

            while (!valid_move) {
                move_dir = rand() % 4;  // Random direction

                if (move_dir == 0 && check_particle_inside(grid, (int32_t[]){pos[0] - 1, pos[1]}) && check_position_clear(grid, (int32_t[]){pos[0] - 1, pos[1]})) {
                    new_pos[0]--;  // Move left
                    valid_move = true;
                } else if (move_dir == 1 && check_particle_inside(grid, (int32_t[]){pos[0] + 1, pos[1]}) && check_position_clear(grid, (int32_t[]){pos[0] + 1, pos[1]})) {
                    new_pos[0]++;  // Move right
                    valid_move = true;
                } else if (move_dir == 2 && check_particle_inside(grid, (int32_t[]){pos[0], pos[1] - 1}) && check_position_clear(grid, (int32_t[]){pos[0], pos[1] - 1})) {
                    new_pos[1]--;  // Move up
                    valid_move = true;
                } else if (move_dir == 3 && check_particle_inside(grid, (int32_t[]){pos[0], pos[1] + 1}) && check_position_clear(grid, (int32_t[]){pos[0], pos[1] + 1})) {
                    new_pos[1]++;  // Move down
                    valid_move = true;
                }
            }

            pos[0] = new_pos[0];
            pos[1] = new_pos[1];

            if (check_particle_adjecent(grid, (int32_t*)pos)) {
                set_particle(grid, (int32_t*)pos);  // Place the particle
                break;  // Exit this particle's loop once placed
            }

            particle_iterations++;
        }

        if (particle_iterations >= grid->iteration_max) {
            set_particle(grid, (int32_t*)pos);  // Force place the particle
        }
    }
}

// Free the grid's allocated memory
void grid_free(grid_t *grid) {
    for (uint32_t i = 0; i < grid->size; i++) {
        free(grid->array[i]);
    }
    free(grid->array);
    free(grid);
}
