#include "grid.h"
#include "_utils.h"

void grid_alloc(grid_t *grid, uint32_t size, float c) {
    grid->size = size;
    if (c >= 1) {
        printf(LOG_TYPE_WARNING);
        printf(LOG_WARNING_GRID_0);
        char m = (char)getchar();
        if (tolower(c) == 'y') {
            printf(LOG_TYPE_INFO);
            printf(LOG_INFO_MSG_0);
            c = 0.5;
        } else {
            printf(LOG_TYPE_INFO);
            printf(LOG_INFO_MSG_1);
            exit(0);
        }
    }

    grid->particle_max = (uint32_t)grid->size/c;

    // Allocate memory for the weigths
    grid->weights = (float*)malloc(sizeof(float) * grid->size);
    if (!grid->weights) {
        printf(LOG_TYPE_ERROR);
        printf(LOG_ERROR_GRID_0);
        exit(-1);
    }

    for (int i = 0; i < grid->size; i++) {
        grid->weights[i] = malloc(sizeof(float) * grid->size);
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

// initialize the starting particles and the starting weights
void grid_init(grid_t *grid, vector_t *start_particles, vector_t start_weight_pos, float* start_weight_val) {
    
}