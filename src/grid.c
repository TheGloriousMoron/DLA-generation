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

    // Fix: Initialize particle count properly
    grid->start_particle_count = start_particle_count; // The number of starting particles
    grid->particle_count = 0; // The number of simulated particles

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
    printf("Initialized grid with a size of %d and a starting particle number of %d and a particle max of %d\n", grid->size, grid->start_particle_count, grid->particle_max);
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
    int attempts = 0; // Add a counter for move attempts
    while (!valid_move && attempts < 100) { // Limit attempts
        int move_dir = rand() % 4;

        switch (move_dir) {
            case (0): // Move Left
                if (particle->x != 0) {
                    particle->x -= 1;
                    valid_move = true;
                }
                break;
            case (1): // Move Right
                if (particle->x < grid->size - 1) {
                    particle->x += 1;
                    valid_move = true;
                }
                break;
            case (2): // Move Up
                if (particle->y != 0) {
                    particle->y -= 1;
                    valid_move = true;
                }
                break;
            case (3): // Move Down
                if (particle->y < grid->size - 1) {
                    particle->y += 1;
                    valid_move = true;
                }
                break;
        }
        attempts++;
    }
    if (attempts >= 100) {
        printf("Particle movement failed after 100 attempts at position (%d, %d)\n", particle->x, particle->y);
    }
}

// Update simulation function
void grid_simulate_particles(grid_t *grid) {
    for (grid->particle_count = grid->start_particle_count; grid->particle_count < grid->particle_max; grid->particle_count++) {
        printf(LOG_TYPE_INFO);
        printf("Running simulation for particle %d out of %d\n", grid->particle_count, grid->particle_max);

        vector_t particle = {
            .x = rand() % grid->size,
            .y = rand() % grid->size
        };

        while (is_occupied(grid, particle.x, particle.y)) {
            particle.x = rand() % grid->size;
            particle.y = rand() % grid->size;
        }

        printf(LOG_TYPE_INFO);
        printf("Particle %d initialized at x %d, y %d\n", grid->particle_count, particle.x, particle.y);

        bool is_settled = false;
        while (!is_settled) {
            // Check if the particle is adjacent to another
            for (int i = 0; i < grid->particle_count; i++) {
                if (grid->particles[i].x == particle.x && grid->particles[i].y == particle.y) {
                    continue; // Skip if it's the same particle
                }
                if (abs(grid->particles[i].x - particle.x) <= 1 && abs(grid->particles[i].y - particle.y) <= 1) {
                    // Check if it is a valid position
                    if (is_occupied(grid, particle.x, particle.y)) {
                        is_settled = true;
                        break;
                    }
                }
            }

            // Move the particle
            move_particle(grid, &particle);
        }

        grid->particles[grid->particle_count] = particle;
        printf(LOG_TYPE_SUCCESS);
        printf("Successfully settled particle %d at x %d y %d\n", grid->particle_count, particle.x, particle.y);
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