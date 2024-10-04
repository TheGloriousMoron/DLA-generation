#ifndef DLA_GENERATION_DEFS_H
#define DLA_GENERATION_DEFS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <lodepng/lodepng.h>
#include <stdarg.h>

#ifdef _WIN32
    #define OUTPATH "C:\\Users\\james\\source\\repos\\dla-generation\\out\\"
    #define LOGPATH "C:\\Users\\james\\source\\repos\\dla-generation\\out\\log\\"
#else
    #define OUTPATH "/workspaces/DLA-generation/out/"
    #define LOGPATH "/workspaces/DLA-generation/out/log/"
#endif


// --DLA-GENERATION-START-- //

typedef struct {
    // paricle states
    int8_t fill_state;
    int8_t adjecent_state;
    int8_t empty_state;

    uint32_t max_particles;
    uint32_t current_particles;

    uint32_t size;

    uint32_t iteration_max;

    int8_t **array;
} grid_t;

grid_t* grid_init(uint32_t size, uint32_t max_particles, uint32_t iteration_max);

bool check_particle_inside(grid_t *grid, int32_t *pos);

bool check_particle_adjecent(grid_t *grid, int32_t *pos);

bool check_position_clear(grid_t *grid, int32_t *p);

void set_particle(grid_t *grid, int32_t *pos);

void grid_run_simulation(grid_t *grid);

void grid_free(grid_t *grid);

// --DLA-GENERATION-END-- //
// --IMAGE-GENERATION-START-- //

static const uint8_t PARTICLE_COLOR[4] = {0xff, 0xff, 0xff, 0xff};
static const uint8_t ADJACENT_COLOR[4] = {0xff, 0xff, 0xff, 0xff};
static const uint8_t EMPTY_COLOR[4] = {0x00, 0x00, 0x00, 0xff};


void save_grid(grid_t *grid, char *filename);
void save_grid(grid_t *grid, char *filename);

#endif