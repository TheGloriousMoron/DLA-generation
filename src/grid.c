#include "grid.h"

const rgba_t white = {255, 255, 255, 255};
const rgba_t black = {0, 0, 0, 255};
const rgba_t particle_color = {255, 0, 255, 255};

void grid_alloc(grid_t *grid, uint32_t size, float c) {
    grid->size = size;

    if (c >= 1.0f) {
        printf(LOG_TYPE_WARNING);
        printf(LOG_WARNING_GRID_0);
        char m = (char)getchar();
        if (tolower(m) == 'y') {  
            printf(LOG_TYPE_INFO);
            printf(LOG_INFO_MSG_0);
            c = 0.5f;
        } else {
            printf(LOG_TYPE_INFO);
            printf(LOG_INFO_MSG_1);
            exit(0);
        }
    }

    grid->particle_max = (uint32_t)((grid->size * grid->size) * c);

    // Allocate memory for weights as a 2D array
    grid->weights = (float**)malloc(sizeof(float*) * grid->size);
    if (!grid->weights) {
        printf(LOG_TYPE_ERROR);
        printf(LOG_ERROR_GRID_0);
        exit(-1);
    }

    for (int i = 0; i < grid->size; i++) {
        grid->weights[i] = (float*)malloc(sizeof(float) * grid->size);
        if (!grid->weights[i]) {
            printf(LOG_TYPE_ERROR);
            printf(LOG_ERROR_GRID_1);
            exit(-1);
        }
    }

    // Allocate memory for the particle array
    grid->particles = (vector_t*)malloc(sizeof(vector_t) * grid->particle_max);
    if (!grid->particles) {
        printf(LOG_TYPE_ERROR);
        printf(LOG_ERROR_GRID_2);
        exit(-1);
    }

    printf(LOG_TYPE_SUCCESS);
    printf(LOG_SUCCESS_GRID_0);
}

void grid_init(grid_t *grid, vector_t *start_particles, uint32_t start_particle_count, vector_t *start_weight_pos, float* start_weight_val, uint32_t start_weight_count) {
    if (start_particle_count >= grid->particle_max) {
        printf(LOG_TYPE_ERROR);
        printf(LOG_ERROR_GRID_3);
        exit(-1);
    }
    if (start_weight_count > grid->size * grid->size) {
        printf(LOG_TYPE_ERROR);
        printf(LOG_ERROR_GRID_4);
        exit(-1);
    }

    grid->particle_count = start_particle_count; // Set particle count correctly
    printf(LOG_TYPE_INFO);
    printf("Starting particle count: %d, Maximum particle count: %d\n", grid->particle_count, grid->particle_max);

    // Set weights
    for (int i = 0; i < start_weight_count; i++) {
        grid->weights[start_weight_pos[i].x][start_weight_pos[i].y] = start_weight_val[i];
    }

    // Set particles
    for (int i = 0; i < start_particle_count; i++) {
        grid->particles[i].x = start_particles[i].x; // Store new particle position
        grid->particles[i].y = start_particles[i].y;
    }

    printf(LOG_TYPE_SUCCESS);
    printf("Initialized grid with a size of %d and a particle number of %d and a particle max of %d\n", grid->size, grid->particle_count, grid->particle_max);
}


bool is_occupied(grid_t *grid, int x, int y) {
    for (int j = 0; j < grid->particle_count; j++) {
        if (grid->particles[j].x == x && grid->particles[j].y == y) {
            return true;
        }
    }
    return false;
}

void move_particle(grid_t *grid, vector_t *particle) {
    bool valid_move = false;
    while (!valid_move) {
        int move_dir = rand() % 4;

        switch (move_dir) {
            case (0): // Move Left
                if (particle->x != 0) {
                    particle->x -= 1;
                    valid_move = true;
                }
                break; // Fixed: Added break
            case (1): // Move Right
                if (particle->x < grid->size - 1) {
                    particle->x += 1;
                    valid_move = true;
                }
                break; // Fixed: Added break
            case (2): // Move Up
                if (particle->y != 0) {
                    particle->y -= 1;
                    valid_move = true;
                }
                break; // Fixed: Added break
            case (3): // Move Down
                if (particle->y < grid->size - 1) {
                    particle->y += 1;
                    valid_move = true;
                }
                break; // Fixed: Added break
            default:
                break;
        }
    }    
}

// Function to simulate particles' movements
void grid_simulate_particles(grid_t *grid) {
    for (int i = grid->particle_count; i < grid->particle_max; i++) {
        printf(LOG_TYPE_INFO);
        printf("Beginning simulation for particle %d out of %d.\n", i, grid->particle_max);
        
        vector_t *particle = (vector_t*)malloc(sizeof(vector_t));
        bool starting_spot_found = false;

        // Generate a starting position for the new particle
        while (!starting_spot_found) {
            particle->x = rand() % grid->size;
            particle->y = rand() % grid->size;

            if (!is_occupied(grid, particle->x, particle->y)) {
                starting_spot_found = true;
            }
        }

        bool is_resting = false;
        while (!is_resting) {
            // Check if the particle is resting (surrounded by others)
            if (is_occupied(grid, particle->x - 1, particle->y) || 
                is_occupied(grid, particle->x + 1, particle->y) ||
                is_occupied(grid, particle->x, particle->y - 1) || 
                is_occupied(grid, particle->x, particle->y + 1)) {
                is_resting = true; // Corrected logic: set is_resting to true
                break; // Particle is resting
            }

            // Move the particle
            move_particle(grid, particle);
        }

        // Save particle's position after movement
        grid->particles[grid->particle_count] = *particle; // Store new particle position
        grid->particle_count++; // Increment particle count
        free(particle); // Free allocated memory for the particle
    }
}

// Function to output a 2D array
rgba_t** grid_get_simulation_data(grid_t *grid) {
    // Loop through all of the particles and set the weight to 2.0
    for (int i = 0; i < grid->particle_count; i++) {
        grid->weights[grid->particles[i].x][grid->particles[i].y] = 2.0f;
    }

    // Create the return array and the colors
    rgba_t **data;
    data = (rgba_t**)malloc(sizeof(rgba_t*) * grid->size);
    for (int i = 0; i < grid->size; i++) {
        data[i] = (rgba_t*)malloc(sizeof(rgba_t) * grid->size);
    }

    // Loop through filling the data;
    for (int c = 0; c < grid->size; c++) {
        for (int r = 0; r < grid->size; r++) {
            // Assign the color to white
            data[c][r] = white;
            data[c][r].a *= grid->weights[c][r];
        }
    }
    
    // Loop through filling the particle data
    for (int i = 0; i < grid->particle_count; i++) {
        data[grid->particles[i].x][grid->particles[i].y] = particle_color;
    }

    return data;
}

void grid_free(grid_t *grid) {
    if (grid->particles) {
        free(grid->particles);
    }
    for (int i = 0; i < grid->size; i++) {
        if (grid->weights[i]) {
            free(grid->weights[i]);
        }
    }
    if (grid->weights) {
        free(grid->weights);
    }

    free(grid);
}