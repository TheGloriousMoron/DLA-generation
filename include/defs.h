#ifndef PROJECT_DEFS_H
#define PROJECT_DEFS_H

#include "includes.h"

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

/*
static const uint8_t PARTICLE_COLOR[4] = {0xff, 0xff, 0xff, 0xff};
static const uint8_t ADJACENT_COLOR[4] = {0x80, 0x80, 0x80, 0xff};
static const uint8_t EMPTY_COLOR[4] = {0x00, 0x00, 0x00, 0xff};

void save_grid_png(grid_t *grid, char *filename);
void save_grid_txt(grid_t *grid, char *filename);
*/

extern const char *help_msg;
#endif