#ifndef _UTILS_H
#define _UTILS_H
#include "includes.h"
#include "defs.h"

char BASEPATH[MAX_PATH_LENGTH];
char OUTPATH[MAX_PATH_LENGTH];
char GRIDPATH[MAX_PATH_LENGTH];

#if defined(_WIN32) || defined(_WIN64)
    void win_get_basepath();
    void win_get_outpath();
    void win_get_gridpath();
#else
    void unix_get_basepath();
    void unix_get_outpath();
    void unix_get_gridpath();
#endif

void get_paths();

/*  .o88b. db      d888888b      db    db d888888b d888888b db      .d8888. */
/* d8P  Y8 88        `88'        88    88 `~~88~~'   `88'   88      88'  YP */
/* 8P      88         88         88    88    88       88    88      `8bo.   */
/* 8b      88         88         88    88    88       88    88        `Y8b. */
/* Y8b  d8 88booo.   .88.        88b  d88    88      .88.   88booo. db   8D */
/*  `Y88P' Y88888P Y888888P      ~Y8888P'    YP    Y888888P Y88888P `8888Y' */

typedef struct {
    bool help;
    uint32_t grid_size;
    float particle_coverage;
    char *out_name;
    bool debug;
    char *grid_out_name;
    bool load_grid;
    char *load_grid_name;
} arguments_t;

// CMD Arguments:
// -h, --help               Print this comments
// -r, --gridsize           grid size
// -p, --particlecount      particles per row
// -o, --output             output file name
// -d, --debug              grid output on/off
// -t, --gridout            grid output filename
// -l, --loadgrid           Advanced options enable grid filename
// -g, --gridloadname       the name of the grid file to be loaded

// Grid File:
// @resolution uint32_t grid size\n
// @count uint32_t particle count\n
// @vector_start\n
// @x uint32_t x\n
// @y uint32_t y\n
// @vector_end\n (for every particle)


arguments_t* args_init(int argc, char **argv);
void free_args(arguments_t *arguments);

/* .88b  d88.  .d8b.  d888888b db   db        */
/* 88'YbdP`88 d8' `8b `~~88~~' 88   88        */
/* 88  88  88 88ooo88    88    88ooo88        */
/* 88  88  88 88~~~88    88    88~~~88        */
/* 88  88  88 88   88    88    88   88        */
/* YP  YP  YP YP   YP    YP    YP   YP        */
/*                                            */
/*                                            */
/* db    db d888888b d888888b db      .d8888. */
/* 88    88 `~~88~~'   `88'   88      88'  YP */
/* 88    88    88       88    88      `8bo.   */
/* 88    88    88       88    88        `Y8b. */
/* 88b  d88    88      .88.   88booo. db   8D */
/* ~Y8888P'    YP    Y888888P Y88888P `8888Y' */

typedef struct {
    int32_t x, y;
} vector_t;

typedef struct {
    float x, y;
} vector_f; // A vector but with floating point values

typedef struct {
    uint8_t r,g,b,a;
} rgba_t;

/* db       .d88b.   d888b       db    db d888888b d888888b db      .d8888. */
/* 88      .8P  Y8. 88' Y8b      88    88 `~~88~~'   `88'   88      88'  YP */
/* 88      88    88 88           88    88    88       88    88      `8bo.   */
/* 88      88    88 88  ooo      88    88    88       88    88        `Y8b. */
/* 88booo. `8b  d8' 88. ~8~      88b  d88    88      .88.   88booo. db   8D */
/* Y88888P  `Y88P'   Y888P       ~Y8888P'    YP    Y888888P Y88888P `8888Y' */

#define LOG_TYPE_ERROR "\033[1;31m[ERROR]\033[0m "
#define LOG_TYPE_SUCCESS "\033[1;32m[SUCCESS]\033[0m "
#define LOG_TYPE_WARNING "\033[1;33m[WARNING]\033[0m "
#define LOG_TYPE_INFO "\033[1;34m[INFO]\033[0m "

#define LOG_INFO_MSG_0 "returning\n   ...\n"
#define LOG_INFO_MSG_1 "exiting\n  ...\n"

/* db       .d88b.   d888b       .88b  d88. .d8888.  d888b        d888b  d8888b. d888888b d8888b. */
/* 88      .8P  Y8. 88' Y8b      88'YbdP`88 88'  YP 88' Y8b      88' Y8b 88  `8D   `88'   88  `8D */
/* 88      88    88 88           88  88  88 `8bo.   88           88      88oobY'    88    88   88 */
/* 88      88    88 88  ooo      88  88  88   `Y8b. 88  ooo      88  ooo 88`8b      88    88   88 */
/* 88booo. `8b  d8' 88. ~8~      88  88  88 db   8D 88. ~8~      88. ~8~ 88 `88.   .88.   88  .8D */
/* Y88888P  `Y88P'   Y888P       YP  YP  YP `8888Y'  Y888P        Y888P  88   YD Y888888P Y8888D' */

#define LOG_ERROR_GRID_0 "Failed to allocate memory for the weight's columns.\n"
#define LOG_ERROR_GRID_1 "Failed to allocate memory for the weight's rows.\n"
#define LOG_ERROR_GRID_2 "Failed to allocate memory for the particle array.\n"
#define LOG_ERROR_GRID_3 "The number of start particles was larger that the number of particles allowed.\n"
#define LOG_ERROR_GRID_4 "The number of starting weights was larger that the number of weights allowed.\n"

#define LOG_SUCCESS_GRID_0 "Succeded to allocate memory for the grid.\n"

#define LOG_WARNING_GRID_0 "Failed coverage value greater or equal to 1. Would you like to continue the generation at 0.5 or exit the program?\n[y/n]\n"
#endif