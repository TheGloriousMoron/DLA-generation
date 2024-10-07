#include "defs.h"

void save_grid_png(grid_t *grid, char *name) {
    // Allocate enough space for the full file path including the null terminator
    size_t filename_len = strlen(OUTPATH) + strlen(name) + 1; // +1 for the null terminator
    char *filename = (char*)malloc(filename_len);

    if (!filename) {
        perror("[ERROR] Unable to allocate memory for filename\n");
        exit(-1);
    }

    // Copy the output path and the name into the filename buffer
    strcpy(filename, OUTPATH);  // Copy the output path first
    strcat(filename, name);     // Append the filename

    // Allocate memory for the image (RGBA - 4 bytes per pixel)
    uint8_t *image = (uint8_t*)malloc(sizeof(uint8_t) * (grid->size * grid->size) * 4);

    if (!image) {
        perror("[ERROR] Unable to allocate memory for image\n");
        free(filename); // Free the filename memory before exiting
        exit(-1);
    }

    printf("[DEBUG] Filling array with grid data\n");

    // Fill the image with the grid data
    for (int r = 0; r < grid->size; r++) {
        for (int c = 0; c < grid->size; c++) {
            // Calculate the offset in row-major order (4 bytes per pixel)
            int offset = (r * grid->size + c) * 4;

            // Check the state of the grid cell and assign the corresponding color
            if (grid->array[r][c] == grid->fill_state) {
                image[offset + 0] = PARTICLE_COLOR[0]; // Red
                image[offset + 1] = PARTICLE_COLOR[1]; // Green
                image[offset + 2] = PARTICLE_COLOR[2]; // Blue
                image[offset + 3] = PARTICLE_COLOR[3]; // Alpha
            } else if (grid->array[r][c] == grid->adjacent_state) {
                image[offset + 0] = ADJACENT_COLOR[0]; // Red
                image[offset + 1] = ADJACENT_COLOR[1]; // Green
                image[offset + 2] = ADJACENT_COLOR[2]; // Blue
                image[offset + 3] = ADJACENT_COLOR[3]; // Alpha
            } else {
                image[offset + 0] = EMPTY_COLOR[0];
                image[offset + 1] = EMPTY_COLOR[1];
                image[offset + 2] = EMPTY_COLOR[2];
                image[offset + 3] = EMPTY_COLOR[3];
            }
        }
    }

    // Encode the image to a PNG file
    unsigned error = lodepng_encode_file(filename, image, grid->size, grid->size, LCT_RGBA, 8);
    if (error) {
        printf("Error %u: %s\n", error, lodepng_error_text(error));
        printf("Error %u: %s\n", error, lodepng_error_text(error));
    }

    // Free allocated memory
    free(image);
    free(filename);
}

void save_grid_txt(grid_t *grid, char *name) {
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
            if (grid->array[x][y] == grid->fill_state) {
                c = '#';
            } else if (grid->array[x][y] == grid->adjacent_state) {
                c = '-';
            } else {
                c = '_';
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