#include "grid.h"

const rgba_t white = {255, 255, 255, 255};
const rgba_t black = {0, 0, 0, 255};
const rgba_t particle_color = {255, 255, 255, 255};

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

    grid->particle_count = (uint32_t)((grid->size * grid->size) * c);
    grid->particle_index = 0;  // Initialize particle index

    // Allocate memory for the particle array
    grid->particles = (vector_t*)malloc(sizeof(vector_t) * grid->particle_count);
    if (!grid->particles) {
        printf(LOG_TYPE_ERROR);
        printf(LOG_ERROR_GRID_2);
        exit(-1);
    }

    printf(LOG_TYPE_SUCCESS);
    printf(LOG_SUCCESS_GRID_0);
}

void grid_init(grid_t *grid, vector_t *start_particles, uint32_t start_particle_count) {
    if (start_particle_count > grid->particle_count) {
        printf(LOG_TYPE_ERROR);
        printf(LOG_ERROR_GRID_3);
        exit(-1);
    }

    grid->particle_index = start_particle_count;

    // Set initial particles
    for (int i = 0; i < start_particle_count; i++) {
        grid->particles[i] = start_particles[i];
    }

    // Add an initial particle if there are none
    if (start_particle_count == 0) {
        grid->particles[0] = (vector_t){rand() % grid->size, rand() % grid->size};
        grid->particle_index = 1;
    }

    printf(LOG_TYPE_SUCCESS);
    printf("Initialized grid with size %d, starting particles: %d, max particles: %d\n",
           grid->size, grid->particle_index, grid->particle_count);
}

void grid_simulate_particles(grid_t *grid) {
    // Simulate particles until reaching max particle limit
    while (grid->particle_index < grid->particle_count) {
        printf(LOG_TYPE_INFO);
        printf("Creating particle %d\n", grid->particle_index);

        // Generate an unoccupied position for the new particle
        vector_t p;
        bool clear = false;
        while (!clear) {
            p = (vector_t){rand() % grid->size, rand() % grid->size};
            clear = true;

            // Verify the spot is unoccupied
            for (int i = 0; i < grid->particle_index; i++) {
                if (grid->particles[i].x == p.x && grid->particles[i].y == p.y) {
                    clear = false;
                    break;
                }
            }
        }

        // Movement loop to find an adjacent position
        while (true) {
            // Check if the particle `p` is adjacent to any other particle
            bool adjacent = false;
            for (int i = 0; i < grid->particle_index; i++) {
                if ((abs(grid->particles[i].x - p.x) <= 1 && grid->particles[i].y == p.y) ||
                    (abs(grid->particles[i].y - p.y) <= 1 && grid->particles[i].x == p.x)) {
                    adjacent = true;
                    break;
                }
            }

            // If adjacent, place the particle and exit the loop
            if (adjacent) {
                grid->particles[grid->particle_index] = p;
                printf(LOG_TYPE_SUCCESS);
                printf("Successfully settled particle %d at x %d, y %d\n", grid->particle_index, p.x, p.y);
                grid->particle_index++;
                break;
            }

            // Randomly move the particle
            uint8_t movedir = rand() % 4; // 0 = up, 1 = down, 2 = left, 3 = right
            switch (movedir) {
                case 0: if (p.y > 0) p.y--; break;
                case 1: if (p.y < grid->size - 1) p.y++; break;
                case 2: if (p.x > 0) p.x--; break;
                case 3: if (p.x < grid->size - 1) p.x++; break;
            }
        }
    }
}


// Function to output a 2D array
rgba_t** grid_get_simulation_data(grid_t *grid) {
    // Loop through all of the particles and set the weight to 2.0
    /*for (int i = 0; i < grid->particle_count; i++) {
        grid->weights[grid->particles[i].x][grid->particles[i].y] = 2.0f;
    }*/

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
    /*for (int i = 0; i < grid->size; i++) {
        if (grid->weights[i]) {
            free(grid->weights[i]);
        }
    }
    if (grid->weights) {
        free(grid->weights);
    }*/

    free(grid);
}