#ifndef DEFS_H
#define DEFS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <lodepng/lodepng.h>

// MATH //

typedef struct {
    int32_t x, y, z;
} vector3_t;

typedef struct {
    int32_t x, y;
} vector2_t;

vector2_t Vector2(int32_t *arr);

bool Vector2_is_equal(vector2_t v1, vector2_t v2);

// --DLA-GENERATION-START-- //

typedef struct {
    uint32_t grid_size;
    vector2_t **particle_array;
    uint8_t particle_num;
    vector2_t particle_empty;
}grid_t;

void grid_constructor(grid_t *grid, uint32_t size, uint8_t particle_num);

void grid_active(grid_t *grid);

void grid_deconstructor(grid_t grid);

// --DLA-GENERATION-END-- //

#define PARTICLE_COLOR {0xff, 0xff, 0xff, 0xff}
#define EMPTY_COLOR {0x00, 0x00, 0x00, 0xff}

typedef struct {
    uint8_t r, g, b, a;
} rgba_t;

void grid_to_raw_rgba(grid_t *grid, rgba_t *rgba_buffer_ptr);
void save_raw_rgba(grid_t *grid, char *file_name);

#endif
