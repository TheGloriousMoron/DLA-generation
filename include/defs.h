#ifndef DLA_GENERATION_DEFS_H
#define DLA_GENERATION_DEFS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <lodepng/lodepng.h>
#include <Windows.h>
#include <stdarg.h>

// Log

typedef struct {
    char *filename;
} log_t;

void log_constructor(log_t *log, const char *filename);

void log_write(log_t *log, const char *format, ...);

void log_deconstructor(log_t *log);  // Fixed typo

// MATH //

typedef struct {
    int32_t x, y, z;
} vector3_t;

typedef struct {
    int32_t x, y;
} vector2_t;

vector2_t Vector2_arr(int32_t *arr);

vector2_t Vector2_int(int32_t x, int32_t y);

bool Vector2_is_equal(vector2_t v1, vector2_t v2);

// --DLA-GENERATION-START-- //

typedef struct {
    uint32_t grid_size;
    vector2_t **particle_array;
    uint32_t particle_num;
    vector2_t particle_empty;
    vector2_t particle_adjecent;
} grid_t;

void grid_constructor(grid_t *grid, uint32_t size, uint32_t particle_num);

void grid_set_particle(grid_t *grid, vector2_t particle);

bool grid_check_particle(grid_t *grid, vector2_t *p);

void grid_active(grid_t *grid);

void grid_deconstructor(grid_t *grid);

// --DLA-GENERATION-END-- //

#define PARTICLE_COLOR {0xff, 0xff, 0xff, 0xff}
#define EMPTY_COLOR {0x00, 0x00, 0x00, 0xff}

void grid_to_png(grid_t *grid, char *filename);

#endif
