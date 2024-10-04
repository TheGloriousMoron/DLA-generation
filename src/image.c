#include "defs.h"

void save_grid(grid_t *grid, char *name) {
    // Allocate memory for the output file path
    char *filename = (char*)malloc(strlen(OUTPATH) + strlen(name) + 1);
    if (!filename) {
        perror("[ERROR] Failed to allocate memory for filename");
        exit(EXIT_FAILURE);
    }

    // Create the full file path
    strcpy(filename, OUTPATH);
    strcat(filename, name);

    // Allocate memory for the image data (RGBA)
    uint8_t *image = (uint8_t*)malloc(sizeof(uint8_t) * (grid->size * grid->size) * 4);
    if (!image) {
        perror("[ERROR] Unable to allocate memory for image");
        free(filename);
        exit(EXIT_FAILURE);
    }

    // Fill the image with grid data (RGBA format)
    for (int r = 0; r < grid->size; r++) {
        for (int c = 0; c < grid->size; c++) {
            // Calculate the offset in row-major order
            int offset = (r * grid->size + c) * 4; // Each pixel has 4 bytes (RGBA)
            
            if (grid->array[r][c] == grid->fill_state) { // Particle
                image[offset + 0] = PARTICLE_COLOR[0]; // Red
                image[offset + 1] = PARTICLE_COLOR[1]; // Green
                image[offset + 2] = PARTICLE_COLOR[2]; // Blue
                image[offset + 3] = PARTICLE_COLOR[3]; // Alpha
            } else if (grid->array[r][c] == grid->adjecent_state) { // Adjacent
                image[offset + 0] = ADJACENT_COLOR[0];
                image[offset + 1] = ADJACENT_COLOR[1];
                image[offset + 2] = ADJACENT_COLOR[2];
                image[offset + 3] = ADJACENT_COLOR[3];
            } else if (grid->array[r][c] == grid->empty_state) { // Empty
                image[offset + 0] = EMPTY_COLOR[0];
                image[offset + 1] = EMPTY_COLOR[1];
                image[offset + 2] = EMPTY_COLOR[2];
                image[offset + 3] = EMPTY_COLOR[3];
            }
        }
    }

    // Use lodepng to encode the image and save to file
    unsigned error = lodepng_encode_file(filename, image, grid->size, grid->size, LCT_RGBA, 8);
    if (error) {
        printf("Error %u: %s\n", error, lodepng_error_text(error));
    }

    // Clean up allocated memory
    free(image);
    free(filename);
}
