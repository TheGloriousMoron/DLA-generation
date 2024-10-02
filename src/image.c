#include "defs.h"

void grid_to_png(grid_t *grid, char *filename) {
    log_t *log = (log_t*)malloc(sizeof(log_t));
    char *log_filename = "C:\\Users\\james\\source\\repos\\DLA-generation\\out\\log\\grid_to_png.txt";
    log_constructor(log, log_filename);

    int grid_size_squared = grid->grid_size * grid->grid_size; // 128 * 128 = 16384
    int array_size = grid_size_squared * 4;                    // 16384 * 4 = 65536
    uint8_t *image = (uint8_t*)malloc(array_size);
    if (!image) {
        log_write(log, "[DEBUG] Failed to allocate memory for image data %d\n", array_size);
        exit(-1);
    } else {
        log_write(log, "[DEBUG] Allocated memory for image data. Size: %d bytes\n", array_size);
    }

    // fill image data
    log_write(log, "[DEBUG] Filling image data\n");
    for (int y = 0; y < grid->grid_size; y++) {
        for (int x = 0; x < grid->grid_size; x++) {
            int offset = (y * grid->grid_size + x) * 4;
            if (Vector2_is_equal(grid->particle_array[x][y], grid->particle_empty)) {
                uint8_t tmp_col[4] = EMPTY_COLOR;
                image[offset + 0] = tmp_col[0]; // r
                image[offset + 1] = tmp_col[1]; // g
                image[offset + 2] = tmp_col[2]; // b
                image[offset + 3] = tmp_col[3]; // a
            } else {
                uint8_t tmp_col[4] = PARTICLE_COLOR;
                image[offset + 0] = tmp_col[0]; // r
                image[offset + 1] = tmp_col[1]; // g
                image[offset + 2] = tmp_col[2]; // b
                image[offset + 3] = tmp_col[3]; // a
            }
        }
    }
    log_write(log, "[DEBUG] Filled image data\n");

    log_write(log, "[DEBUG] Saving file\n");
    unsigned error = lodepng_encode32_file(filename, image, grid->grid_size, grid->grid_size);

    if (error) {
        log_write(log, "Error in saving file. Error: %u: %s\n", error, lodepng_error_text(error));
    } else {
        log_write(log, "Saved file\n");
    }

    free(image);

    log_deconstructor(log);
}