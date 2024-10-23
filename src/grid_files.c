#include "grid.h"

void save_grid_png(grid_t *grid, rgba_t** data, char *name) {
    // Allocate enough space for the full file path including the null terminator
    size_t filename_len = strlen(OUTPATH) + strlen(name) + 1; // +1 for the null terminator
    char *filename = (char*)malloc(filename_len);

    if (!filename) {
        printf(LOG_TYPE_ERROR);
        printf("Unable to allocate memory for filename\n");
        exit(-1);
    }

    // Copy the output path and the name into the filename buffer
    strcpy(filename, OUTPATH);  // Copy the output path first
    strcat(filename, name);     // Append the filename

    // Allocate memory for the image (RGBA - 4 bytes per pixel)
    uint8_t *image = (uint8_t*)malloc(sizeof(uint8_t) * (grid->size * grid->size) * 4);

    if (!image) {
        printf(LOG_TYPE_ERROR);
        printf("Unable to allocate memory for image\n");
        free(filename); // Free the filename memory before exiting
        exit(-1);
    }

    printf(LOG_TYPE_INFO);
    printf("Filling array with grid data\n");

    // Fill the image with the rgba data row major order
    for (int r = 0; r < grid->size; r++) {
        for (int c = 0; c < grid->size; c++) {
            int index = (r * grid->size + c) * 4; // Correct indexing
            image[index + 0] = data[r][c].r; // Red
            image[index + 1] = data[r][c].g; // Green
            image[index + 2] = data[r][c].b; // Blue
            image[index + 3] = data[r][c].a; // Alpha
        }
    }

    // Encode the image to a PNG file
    unsigned error = lodepng_encode_file(filename, image, grid->size, grid->size, LCT_RGBA, 8);
    if (error) {
        printf(LOG_TYPE_ERROR);
        printf("%u: %s\n", error, lodepng_error_text(error));
    }

    // Free allocated memory
    free(image);
    free(filename);
}

void save_grid_txt(grid_t *grid, rgba_t** data, char *name) {
    // Allocate enough space for the full file path, including the path separator and null terminator
    size_t filename_len = strlen(OUTPATH) + strlen(name) + 1; // +1 for null terminator
    char *filename = (char*)malloc(filename_len);

    if (!filename) {
        perror("[ERROR] Unable to allocate memory for filename\n");
        exit(-1);
    }

    // Copy the output path and the name into the filename buffer
    strcpy(filename, OUTPATH);  // Copy the output path first
    strcat(filename, name);     // Append the filename

    // Open the file for writing
    FILE *fptr = fopen(filename, "w");
    if (!fptr) {
        perror("[ERROR] Failed to open file for writing\n");
        free(filename);
        exit(-1);
    }

    // Write the grid content to the file
    for (int x = 0; x < grid->size; x++) {
        for (int y = 0; y < grid->size; y++) {
            // Turn the particle into a character
            char c;
            if (
                data[x][y].r == particle_color.r &&
                data[x][y].g == particle_color.g &&
                data[x][y].b == particle_color.b
                ) {
                c = '#';
            } else {
                c = '-';
            }

            // Write a single character
            fputc(c, fptr);
        }
        // Write a newline after each row
        fputc('\n', fptr);
    }

    fclose(fptr);
    free(filename);
}

void load_grid_yaml(const char *name, grid_data_t *data) {
    printf(LOG_TYPE_INFO);
    printf("Entering the load_grid_yaml function\n");

    size_t filename_len = strlen(GRIDPATH) + strlen(name) + 1; // +1 for the null terminator
    char *filename = (char*)malloc(filename_len);
    if (filename == NULL) {
        printf(LOG_TYPE_ERROR);
        printf("Failed to allocate memory for the grid filename.\n");
        exit(-1);
    }

    strcpy(filename, GRIDPATH);
    strcat(filename, name);

    printf(LOG_TYPE_INFO);
    printf("Grid filepath: %s\n", filename);

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf(LOG_TYPE_ERROR);
        printf("Failed to open grid file.\n");
        free(filename);
        exit(-1);
    }

    yaml_parser_t parser;
    if (!yaml_parser_initialize(&parser)) {
        printf(LOG_TYPE_ERROR);
        printf("Failed to initialize parser\n");
        free(filename);
        fclose(fp);
        exit(-1);
    }

    yaml_parser_set_input_file(&parser, fp);

    // Initialize the grid_data structure
    uint32_t size = 0, particle_count = 0;

    while (1) {
        yaml_event_t event;
        yaml_parser_parse(&parser, &event);

        if (event.type == YAML_DOCUMENT_END_EVENT) {
            yaml_event_delete(&event);
            break; // End of the document
        }

        // Handle size
        if (event.type == YAML_SCALAR_EVENT && strcmp((char *)event.data.scalar.value, "resolution") == 0) {
            yaml_parser_parse(&parser, &event);
            size = atoi((char *)event.data.scalar.value);
        }

        // Handle particle_count
        if (event.type == YAML_SCALAR_EVENT && strcmp((char *)event.data.scalar.value, "particle count") == 0) {
            yaml_parser_parse(&parser, &event);
            particle_count = atoi((char *)event.data.scalar.value);
        }

        // Handle particles (sequence of vectors)
        if (event.type == YAML_SEQUENCE_START_EVENT) {
            data->particles = (vector_t*)malloc(sizeof(vector_t) * particle_count);
            if (data->particles == NULL) {
                printf(LOG_TYPE_ERROR);
                printf("Failed to allocate memory for particle array\n");
                yaml_parser_delete(&parser);
                fclose(fp);
                free(filename);
                exit(-1);
            }

            for (uint32_t i = 0; i < particle_count; i++) {
                yaml_parser_parse(&parser, &event);

                if (event.type == YAML_MAPPING_START_EVENT) {
                    vector_t v = {0, 0};

                    while (1) {
                        yaml_parser_parse(&parser, &event);
                        if (event.type == YAML_MAPPING_END_EVENT) {
                            break; // End of the mapping
                        }

                        if (event.type == YAML_SCALAR_EVENT) {
                            char *key = (char *)event.data.scalar.value;

                            yaml_parser_parse(&parser, &event); // Get the value

                            if (strcmp(key, "x") == 0) {
                                v.x = atoi((char *)event.data.scalar.value);
                            } else if (strcmp(key, "y") == 0) {
                                v.y = atoi((char *)event.data.scalar.value);
                            }
                        }
                    }

                    // Store the vector in the particles array
                    data->particles[i] = v;
                }

                yaml_event_delete(&event);
            }
        }

        yaml_event_delete(&event);
    }

    // Assign size and particle_count to data
    data->size = size;
    data->particle_count = particle_count;

    printf(LOG_TYPE_INFO);
    printf("Grid size loaded: %d\n", data->size);
    printf(LOG_TYPE_INFO);
    printf("Particle count loaded: %d\n", data->particle_count);

    for (int i = 0; i < data->particle_count; i++) {
        printf(LOG_TYPE_INFO);
        printf("Particle %d has an x of %d and a y of %d\n", i, data->particles[i].x, data->particles[i].y);
    }

    // Clean up
    yaml_parser_delete(&parser);
    fclose(fp);
    free(filename);
}


