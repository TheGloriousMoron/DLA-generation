#include "_utils.h"

arguments_t* args_init(int argc, char **argv) {
    // create the argument struct pointer
    arguments_t* args = malloc(sizeof(arguments_t));

    // Initialize the flags
    bool gs = false, pc = false, on = false, dg = false, go = false, hp = false;
    char *gs_arg = NULL, *pc_arg = NULL, *on_arg = NULL, *go_arg = NULL;

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
                    case 's':
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
                    case 'g': 
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
            args->particle_count = atoi(pc_arg);
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
    }
    
    // Free allocated temporary strings
    free(gs_arg);
    free(pc_arg);
    free(on_arg);
    free(go_arg);

    return args;
}

void free_args(arguments_t *args) {
    if (args->out_name) {
        free(args->out_name);
    }
    if (args->grid_out_name) {
        free(args->grid_out_name);
    }

    free(args);
}

// TODO: add logger