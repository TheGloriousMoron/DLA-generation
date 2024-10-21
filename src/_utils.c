#include "_utils.h"

arguments_t* args_init(int argc, char **argv) {
    // create the argument struct pointer
    arguments_t* args = malloc(sizeof(arguments_t));

    // Initialize the flags
    bool gs = false, pc = false, on = false, dg = false, go = false, hp = false, lg = false, gf = false;
    char *gs_arg = NULL, *pc_arg = NULL, *on_arg = NULL, *go_arg = NULL, *gf_arg = NULL;

    // Loop through the arguments to see what args we have
    for (int i = 1; i < argc; i++) {  // i < argc, not i < argc + 1
        if (argv[i][0] == '-') {
            if (argv[i][1] == '-') {
                // TODO: add long argument functionality
            } else {
                switch (argv[i][1]) {
                    case 'h':
                        if (hp) {
                            printf("[ERROR] Inputted the same arg more than once.\n");
                            exit(-1);
                        } else {
                            hp = true;
                            break;
                        }
                    case 'r':
                        if (gs) {
                            printf("[ERROR] Inputted the same arg more than once.\n");
                            exit(-1);
                        } else {
                            if (i + 1 >= argc) {  // Check if next arg exists
                                printf("[ERROR] No argument provided for -s\n");
                                exit(-1);
                            }
                            gs = true;
                            gs_arg = malloc(strlen(argv[i + 1]) + 1);  // Correct allocation size
                            strcpy(gs_arg, argv[i + 1]);
                            i++;  // Skip the next argument, as it has been processed
                            break;
                        }
                    case 'p':
                        if (pc) {
                            printf("[ERROR] Inputted the same arg more than once.\n");
                            exit(-1);
                        } else {
                            if (i + 1 >= argc) {
                                printf("[ERROR] No argument provided for -p\n");
                                exit(-1);
                            }
                            pc = true;
                            pc_arg = malloc(strlen(argv[i + 1]) + 1);
                            strcpy(pc_arg, argv[i + 1]);
                            i++;
                            break;
                        }
                    case 'o': 
                        if (on) {
                            printf("[ERROR] Inputted the same arg more than once.\n");
                            exit(-1);
                        } else {
                            if (i + 1 >= argc) {
                                printf("[ERROR] No argument provided for -o\n");
                                exit(-1);
                            }
                            on = true;
                            on_arg = malloc(strlen(argv[i + 1]) + 1);
                            strcpy(on_arg, argv[i + 1]);
                            i++;
                            break;
                        }
                    case 'd': 
                        if (dg) {
                            printf("[ERROR] Inputted the same arg more than once.\n");
                            exit(-1);
                        } else {
                            dg = true;
                            break;
                        }
                    case 't': 
                        if (go) {
                            printf("[ERROR] Inputted the same arg more than once.\n");
                            exit(-1);
                        } else {
                            if (i + 1 >= argc) {
                                printf("[ERROR] No argument provided for -g\n");
                                exit(-1);
                            }
                            go = true;
                            go_arg = malloc(strlen(argv[i + 1]) + 1);
                            strcpy(go_arg, argv[i + 1]);
                            i++;
                            break;
                        }
                    case 'l':
                        if (lg) {
                            printf("[ERROR] Inputted the same arg more than once.\n");
                            exit(-1);
                        } else {
                            lg = true;
                            break;
                        }
                    case 'g':
                        if (gf) {
                            printf("[ERROR] Inputted the same arg more than once.\n");
                            exit(-1);
                        } else {
                            gf = true;
                            gf_arg = malloc(strlen(argv[i+1]) + 1);
                            strcpy(gf_arg, argv[i+1]);
                            i++;
                            break;
                        }
                    default:
                        printf("[ERROR] Unknown argument: %s\n", argv[i]);
                        exit(-1);
                }
            }
        }
    }

    // assign all of the values
    if (hp) {
        args->help = true;
    } else {
        args->help = false;
        if (!gs || !pc || !on) {
            printf("[ERROR] Required args not found\n");
            printf("use -h for help\n");
            exit(-1);
        } else {
            args->grid_size = atoi(gs_arg);
            args->particle_coverage = atof(pc_arg);
            args->out_name = malloc(strlen(on_arg) + 1);
            strcpy(args->out_name, on_arg);
        }

        if (dg) {
            if (!go) {
                printf("[ERROR] No filename for the grid output\n");
                exit(-1);
            } else {
                args->debug = true;
                args->grid_out_name = malloc(strlen(go_arg) + 1);
                strcpy(args->grid_out_name, go_arg);
            }
        } else {
            args->debug = false;
        }
        if (lg) {
            if (!gf) {
                printf("[ERROR] No filename to load grid from\n");
                exit(-1);
            } else {
                args->load_grid = true;
                args->load_grid_name = malloc(strlen(gf_arg) + 1);
                strcpy(args->load_grid_name, gf_arg);
            }
        }
    }
    
    // Free allocated temporary strings
    free(gs_arg);
    free(pc_arg);
    free(on_arg);
    free(go_arg);
    free(gf_arg);

    return args;
}

void free_args(arguments_t *args) {
    if (args->out_name) {
        free(args->out_name);
    }
    if (args->grid_out_name) {
        free(args->grid_out_name);
    }
    if (args->load_grid_name) {
        free(args->load_grid_name);
    }

    free(args);
}

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