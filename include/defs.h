#ifndef PROJECT_DEFS_H
#define PROJECT_DEFS_H

#include "includes.h"

#define MAX_PATH_LENGTH 1024

extern char BASEPATH[MAX_PATH_LENGTH];
extern char OUTPATH[MAX_PATH_LENGTH];
extern char GRIDPATH[MAX_PATH_LENGTH];

#ifdef _WIN32
    // Function to get the output path dynamically in Windows
    void win_get_basepath();
    void win_get_outpath();
    void win_get_gridpath();
#else
    // Function to get the output path dynamically in Unix-based systems
    void unix_get_basepath();
    void unix_get_outpath();
    void unix_get_gridpath();
#endif

void get_paths();

extern const char *help_msg;
#endif