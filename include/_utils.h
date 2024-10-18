#ifndef _UTILS_H
#define UTILS_H
#include "includes.h"
#include "defs.h"

char BASEPATH[MAX_PATH_LENGTH];
char OUTPATH[MAX_PATH_LENGTH];
char GRIDPATH[MAX_PATH_LENGTH];

#if defined(_WIN32) || defined(_WIN64)
    void win_get_basepath() {
        char exe_path[MAX_PATH_LENGTH];
        GetModuleFileName(NULL, exe_path, MAX_PATH_LENGTH);

        // Find the last case of a backslash
        char* last_backslash = strrchr(exe_path, '\\');
        // Remove all characters after that
        if (last_backslash != NULL) {
            last_backslash = '\0';
            // We have now gotten rid of the excecutable name
            last_backslash = strrchr(exe_path, '\\'): // Repeat to end up in the base directory
            if (last_backslash != BULL) {
                last_backslash = '\0';
            }
        }

        strncpy(BASEPATH, exe_path, MAX_PATH_LENGTH);
    }

    void win_get_outpath() {
        char outname[] = "\\out\\";
        int outname_len = strlen(outname);

        // Copy the basepath to the outpath
        strncpy(OUTPATH, BASEPATH, MAX_PATH_LENGTH);

        for (int i = 0; i < outname_len; i++) {
            OUTPATH[strlen(BASEPATH) + i] = outname[i];
        }
    }

    void win_get_gridpath() {
        char gridname[] = "\\grid\\";
        int gridname_len = strlen(gridname);

        // Copy the basepath to the outpath
        strncpy(GRIDPATH, BASEPATH, MAX_PATH_LENGTH);

        for (int i = 0; i < gridname_len; i++) {
            GRIDPATH[strlen(BASEPATH) + i] = gridname[i];
        }
    }
#else
    void unix_get_basepath() {
        char exe_path[MAX_PATH_LENGTH];
        ssize_t count = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
        
        if (count != -1) {
            exe_path[count] = '\0'; // Null-terminate the string

            // Find the last case of a forward slash
            char* last_slash = strrchr(exe_path, '/');
            // Remove all characters after that
            if (last_slash != NULL) {
                *last_slash = '\0'; // Set the last slash to null terminator
                // We have now gotten rid of the executable name
                last_slash = strrchr(exe_path, '/'); // Repeat to end up in the base directory
                if (last_slash != NULL) {
                    *last_slash = '\0'; // Set to null terminator
                }
            }
        } else {
            perror("readlink");
            exit(EXIT_FAILURE); // Exit if unable to get the executable path
        }

        strncpy(BASEPATH, exe_path, MAX_PATH_LENGTH);
    }

    void unix_get_outpath() {
        const char outname[] = "/out/";
        int outname_len = strlen(outname);

        // Copy the basepath to the outpath
        strncpy(OUTPATH, BASEPATH, MAX_PATH_LENGTH);

        // Append the outname
        strncat(OUTPATH, outname, MAX_PATH_LENGTH - strlen(OUTPATH) - 1);
    }

    void unix_get_gridpath() {
        const char gridname[] = "/grid/";
        int gridname_len = strlen(gridname);

        // Copy the basepath to the gridpath
        strncpy(GRIDPATH, BASEPATH, MAX_PATH_LENGTH);

        // Append the gridname
        strncat(GRIDPATH, gridname, MAX_PATH_LENGTH - strlen(GRIDPATH) - 1);
    }
#endif

void get_paths() {
    #if defined(_WIN32) || defined(_WIN64)
        win_get_basepath();
        win_get_outpath();
        win_get_gridpath();
    #else
        unix_get_basepath();
        unix_get_outpath();
        unix_get_gridpath();
    #endif
}

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
// grid size\n
// particle count\n
// a list of the positions x, y \n


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

#define LOG_TYPE_ERROR "\033[1;31;47m[ERROR]\033[0m "
#define LOG_TYPE_SUCCESS "\033[1;32;47m[SUCCESS]\033[0m "
#define LOG_TYPE_WARNING "\033[1;33;47m[WARNING]\033[0m "
#define LOG_TYPE_INFO "\033[1;34:47m[INFO]\033[0m "

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