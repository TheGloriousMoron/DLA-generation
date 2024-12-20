#ifndef GRID_H
#define GRID_H
#include "includes.h"
#include "_utils.h"
#include "defs.h"
#include "math.h"

extern const rgba_t white;
extern const rgba_t black;
extern const rgba_t particle_color;

typedef struct {
    uint32_t size;
    uint32_t particle_count;
    vector_t *particles;
    uint32_t particle_index, start_particle_count;
} grid_t;

typedef struct {
    uint32_t size, particle_count;
    vector_t *particles;
} grid_data_t;

void grid_alloc(grid_t *grid, uint32_t size, float c);
void grid_init(grid_t *grid, vector_t *start_particles, uint32_t start_particle_count);
void grid_simulate_particles(grid_t *grid);
void move_particle(grid_t *grid, vector_t *p);
void grid_free(grid_t *grid);
rgba_t** grid_get_simulation_data(grid_t *grid);
void save_grid_png(grid_t *grid, rgba_t** data, char *name);
void save_grid_yaml(grid_t *grid, char *name);


void load_grid_yaml(const char *name, grid_data_t *data);
void scale_grid_yaml(uint32_t new_size, uint32_t old_size, uint32_t* particle_count, vector_t *particles);

#endif