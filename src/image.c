#include "defs.h"

void save_grid(grid_t *grid, char *name) {
    // Allocate enough space for the full file path including the null terminator
    size_t filename_len = strlen(OUTPATH) + strlen(name) + 1; // +1 for the null terminator
    char *filename = (char*)malloc(filename_len);

    if (!filename) {
        perror("[ERROR] Unable to allocate memory for filename");
        exit(-1);
    }

    // Copy the output path and the name into the filename buffer
    strcpy(filename, OUTPATH);  // Copy the output path first
    strcat(filename, name);     // Append the filename

    // Allocate memory for the image (RGBA - 4 bytes per pixel)
    uint8_t *image = (uint8_t*)malloc(sizeof(uint8_t) * (grid->size * grid->size) * 4);

    if (!image) {
        perror("[ERROR] Unable to allocate memory for image");
        free(filename); // Free the filename memory before exiting
        exit(-1);
    }

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
            } else if (grid->array[r][c] == grid->adjecent_state) {
                image[offset + 0] = ADJACENT_COLOR[0];
                image[offset + 1] = ADJACENT_COLOR[1];
                image[offset + 2] = ADJACENT_COLOR[2];
                image[offset + 3] = ADJACENT_COLOR[3];
            } else if (grid->array[r][c] == grid->empty_state) {
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
