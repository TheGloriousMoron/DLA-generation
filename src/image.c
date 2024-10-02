#include "defs.h" // src/image.c

void grid_to_raw_rgba(grid_t *grid, rgba_t *rgba_buffer) {
    // Convert the grid to the rgba_buffer in row-major order
    for (int y = 0; y < grid->resolution; y++) {
        for (int x = 0; x < grid->resolution; x++) {
            uint32_t index = y * grid->resolution + x;  // Directly calculate the array index
            if (grid->particles[x][y].posx != -1) {
                rgba_buffer[index] = (rgba_t)PARTICLE_COLOR;  // Assign particle color
            } else {
                rgba_buffer[index] = (rgba_t)EMPTY_COLOR;  // Assign empty color
            }
        }
    }
    // The raw RGBA data is now in rgba_buffer in row-major order
}


void save_raw_rgba(grid_t *grid, char *file_name) {
    rgba_t *raw_rgb = malloc(grid->resolution * grid->resolution * sizeof(rgba_t));  // Allocate memory for raw RGB data
    if (!raw_rgb) {
        printf("Failed to allocate memory for raw RGBA data\n");
        return;
    }

    grid_to_raw_rgba(grid, raw_rgb);  // Assuming this function fills the raw_rgb buffer

    uint8_t *png_encoded = NULL;  // This will hold the encoded PNG data
    size_t png_size = 0;  // size_t is the correct type for png_size

    // Encode raw RGBA to PNG format
    unsigned error = lodepng_encode32(&png_encoded, &png_size, (unsigned char *)raw_rgb, grid->resolution, grid->resolution);
    if (error) {
        printf("Error encoding PNG: %s\n", lodepng_error_text(error));
        free(raw_rgb);
        return;
    }

    // Save the PNG file
    error = lodepng_save_file(png_encoded, png_size, file_name);
    if (error) {
        printf("Error saving PNG: %s\n", lodepng_error_text(error));
    } else {
        printf("PNG file saved successfully: %s\n", file_name);
    }

    // Free allocated memory
    free(png_encoded);
    free(raw_rgb);
}
