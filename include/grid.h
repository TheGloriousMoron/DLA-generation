#include "includes.h"
#include "_utils.h"

typedef struct {
    uint32_t size;
    uint32_t particle_max;
    float **weights;
    vector_t *particles;
} grid_t;

void grid_alloc(grid_t *grid, uint32_t size, float c);