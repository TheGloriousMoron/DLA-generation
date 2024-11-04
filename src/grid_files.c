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

void scale_grid_yaml(uint32_t new_size, uint32_t old_size, uint32_t* particle_count, vector_t *particles) {
    float scale_factor = (float)new_size / (float)old_size;
    uint32_t original_count = *particle_count;

    // Allocate temporary weight grid for smoothing
    float **w = (float**)malloc(sizeof(float*) * new_size);
    if (w == NULL) {
        printf("Error: Failed to allocate memory for grid weights.\n");
        exit(-1);
    }
    for (uint32_t i = 0; i < new_size; i++) {
        w[i] = (float*)calloc(new_size, sizeof(float));
        if (w[i] == NULL) {
            printf("Error: Failed to allocate memory for grid weights.\n");
            exit(-1);
        }
    }

    // Scale each particle and distribute weights
    for (uint32_t i = 0; i < original_count; i++) {
        int32_t new_x = (int32_t)(particles[i].x * scale_factor);
        int32_t new_y = (int32_t)(particles[i].y * scale_factor);

        // Clamp within bounds
        new_x = new_x < 0 ? 0 : (new_x >= (int32_t)new_size ? new_size - 1 : new_x);
        new_y = new_y < 0 ? 0 : (new_y >= (int32_t)new_size ? new_size - 1 : new_y);

        // Apply smoothing weights
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                int nx = new_x + dx;
                int ny = new_y + dy;
                if (nx >= 0 && nx < (int32_t)new_size && ny >= 0 && ny < (int32_t)new_size) {
                    float distance = sqrtf(dx * dx + dy * dy);
                    float weight = 1.0f / (1.0f + distance);
                    w[nx][ny] += weight;
                }
            }
        }
    }

    // Normalize weights
    float max_weight = 0.0f;
    for (uint32_t x = 0; x < new_size; x++) {
        for (uint32_t y = 0; y < new_size; y++) {
            if (w[x][y] > max_weight) {
                max_weight = w[x][y];
            }
        }
    }
    if (max_weight > 0.0f) {
        for (uint32_t x = 0; x < new_size; x++) {
            for (uint32_t y = 0; y < new_size; y++) {
                w[x][y] /= max_weight;
            }
        }
    }

    // Deallocate weight grid memory
    for (uint32_t i = 0; i < new_size; i++) {
        free(w[i]);
    }
    free(w);

    // Update the particle count based on scaling factor
    uint32_t scaled_particle_count = (uint32_t)(original_count * scale_factor * scale_factor);
    *particle_count = scaled_particle_count <= original_count ? scaled_particle_count : original_count;

    printf(LOG_TYPE_INFO);
    printf("Scaled grid to size %d with new particle count %d\n", new_size, *particle_count);
}

void save_grid_yaml(grid_t *grid, char *name) {
    // Allocate enough space for the full file path, including the path separator and null terminator
    size_t filename_len = strlen(GRIDPATH) + strlen(name) + 2; // +2 for path separator and null terminator
    char *filename = (char*)malloc(filename_len);

    if (!filename) {
        printf(LOG_TYPE_ERROR);
        printf("Unable to allocate memory for filename\n");
        exit(-1);
    }

    // Construct the filename
    snprintf(filename, filename_len, "%s/%s.yaml", GRIDPATH, name); // Use snprintf for safety

    // Open the file for writing
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf(LOG_TYPE_ERROR);
        printf("Unable to open file %s for writing\n", filename);
        free(filename);
        exit(-1);
    }

    // Write YAML header
    fprintf(file, "resolution: %d\n", grid->size);
    fprintf(file, "particle count: %d\n", grid->particle_count);
    fprintf(file, "particles:\n");

    // Write particle positions
    for (uint32_t i = 0; i < grid->particle_count; i++) {
        fprintf(file, "   - x: %d\n", grid->particles[i].x);
        fprintf(file, "     y: %d\n", grid->particles[i].y);
    }

    // Clean up
    fclose(file);
    free(filename);

    printf(LOG_TYPE_SUCCESS);
    printf("Grid data saved successfully to %s\n", filename);
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


