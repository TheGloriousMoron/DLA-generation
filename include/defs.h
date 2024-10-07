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
    #include <windows.h>
#else
    #include <unistd.h>
    #include <sys/types.h> // For ssize_t
#endif

#define MAX_PATH_LENGTH 1024

extern char OUTPATH[MAX_PATH_LENGTH];
extern char LOGPATH[MAX_PATH_LENGTH];

#ifdef _WIN32
    // Function to get the output path dynamically in Windows
    void win_get_outpath(char *outpath);
#else
    // Function to get the output path dynamically in Unix-based systems
    void unix_get_outpath(char *outpath);
#endif

// --DLA-GENERATION-START-- //

typedef struct {
    int8_t fill_state;
    int8_t adjacent_state;
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
static const uint8_t ADJACENT_COLOR[4] = {0x80, 0x80, 0x80, 0xff};
static const uint8_t EMPTY_COLOR[4] = {0x00, 0x00, 0x00, 0xff};

void save_grid_png(grid_t *grid, char *filename);
void save_grid_txt(grid_t *grid, char *filename);

#endif
