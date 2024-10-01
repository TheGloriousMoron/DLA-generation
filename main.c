#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <png.h>  // Include the libpng header

// Particle structure representing each particle
typedef struct {
    int32_t posx, posy;
} particle_t;

// Grid structure for holding particle positions
typedef struct {
    uint32_t resolution;
    particle_t **particles;
    particle_t particle_null;
} grid_t;

// Function to generate the CRC32 lookup table
void generate_crc32_table() {
    // Not needed when using libpng
}

// Function to initialize the grid with empty particles
void grid_init(grid_t *grid, uint32_t resolution, particle_t origin_particle, particle_t particle_null) {
    printf("[DEBUG] Initializing grid with resolution %u...\n", resolution);
    grid->resolution = resolution;

    // Allocate memory for rows
    grid->particles = malloc(resolution * sizeof(particle_t*));
    if (!grid->particles) {
        perror("Failed to allocate memory for grid rows");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for each row and initialize all particles to particle_null
    for (int i = 0; i < resolution; i++) {
        grid->particles[i] = malloc(grid->resolution * sizeof(particle_t));
        if (!grid->particles[i]) {
            perror("Failed to allocate memory for grid particles");
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < grid->resolution; j++) {
            grid->particles[i][j] = particle_null;
        }
    }

    grid->particle_null = particle_null;

    // Set the origin particle in the grid
    if (origin_particle.posx < resolution && origin_particle.posy < resolution) {
        grid->particles[origin_particle.posx][origin_particle.posy] = origin_particle;
        printf("[DEBUG] Origin particle set at (%d, %d)\n", origin_particle.posx, origin_particle.posy);
    } else {
        printf("Error initializing grid: the origin particle is outside of the grid\n");
    }
}

// Function to initialize a particle at a position
void particle_init(particle_t *particle, int32_t x, int32_t y, grid_t *grid) {
    particle->posx = x;
    particle->posy = y;
    if (particle->posx < 0 || particle->posx >= grid->resolution || particle->posy < 0 || particle->posy >= grid->resolution) {
        particle->posx = rand() % grid->resolution;
        particle->posy = rand() % grid->resolution;
    }
    printf("[DEBUG] Particle initialized at (%d, %d)\n", particle->posx, particle->posy);
}

// Function to initialize the grid and place the origin particle
void dla_init(grid_t *grid, particle_t *particle_null, uint32_t grid_res) {
    srand(time(NULL));
    particle_null->posx = -1;
    particle_null->posy = -1;

    printf("[DEBUG] Initializing DLA grid with resolution %u...\n", grid_res);

    // Allocate and initialize the origin particle
    particle_t origin_particle;
    particle_init(&origin_particle, rand() % grid_res, rand() % grid_res, grid);

    // Initialize the grid
    grid_init(grid, grid_res, origin_particle, *particle_null);

    printf("[DEBUG] DLA grid initialized.\n");
}

// Function to move a particle within the grid until it sticks
void particle_move(grid_t *grid, particle_t *particle, bool *particle_done) {
    int x_vel, y_vel;
    x_vel = (rand() % 3) - 1;  // Random velocity between -1, 0, 1
    y_vel = (rand() % 3) - 1;

    // Adjust velocity if out of bounds
    if (particle->posx + x_vel < 0 || particle->posx + x_vel >= grid->resolution) x_vel = 0;
    if (particle->posy + y_vel < 0 || particle->posy + y_vel >= grid->resolution) y_vel = 0;

    // Check if particle can stick to existing particles in the grid
    for (int i = 0; i < grid->resolution; i++) {
        for (int j = 0; j < grid->resolution; j++) {
            if (!(*particle_done)) {
                if (grid->particles[i][j].posx != grid->particle_null.posx || grid->particles[i][j].posy != grid->particle_null.posy) {
                    if (particle->posx + x_vel == grid->particles[i][j].posx && particle->posy + y_vel == grid->particles[i][j].posy) {
                        *particle_done = true;
                        grid->particles[particle->posx][particle->posy] = *particle;
                        printf("[DEBUG] Particle stuck at (%d, %d)\n", particle->posx, particle->posy);
                    }
                }
            }
        }
    }

    // Move the particle if it hasn't stuck
    if (!(*particle_done)) {
        particle->posx += x_vel;
        particle->posy += y_vel;
    }
}

// Function to generate the image data based on particle positions
void generate_image_data(grid_t *grid, uint8_t **image_data) {
    // Allocate image data (RGBA format)
    *image_data = malloc(grid->resolution * grid->resolution * 4);
    if (!*image_data) {
        perror("Failed to allocate memory for image data");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < grid->resolution; i++) {
        for (uint32_t j = 0; j < grid->resolution; j++) {
            uint8_t *pixel = &(*image_data)[(i * grid->resolution + j) * 4];

            // Check if there is a particle at this position
            if (grid->particles[i][j].posx != grid->particle_null.posx || grid->particles[i][j].posy != grid->particle_null.posy) {
                // Particle is present, set pixel to black (RGBA)
                pixel[0] = 0;   // R
                pixel[1] = 0;   // G
                pixel[2] = 0;   // B
                pixel[3] = 255; // A (fully opaque)
            } else {
                // No particle, set pixel to white (RGBA)
                pixel[0] = 255; // R
                pixel[1] = 255; // G
                pixel[2] = 255; // B
                pixel[3] = 255; // A (fully opaque)
            }
        }
    }
}

// Generate a PNG file using libpng
void make_png(const char *filename, uint8_t *image_data, uint32_t width, uint32_t height) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("Failed to open file for writing PNG");
        return;
    }

    // Create the PNG write struct
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fclose(fp);
        perror("Failed to create PNG write struct");
        return;
    }

    // Create the PNG info struct
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, NULL);
        fclose(fp);
        perror("Failed to create PNG info struct");
        return;
    }

    // Set error handling
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        perror("Failed to set PNG error handling");
        return;
    }

    // Initialize the PNG writing
    png_init_io(png_ptr, fp);

    // Set the PNG header information (correct usage)
    png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGBA,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    
    // Write the info to the PNG file
    png_write_info(png_ptr, info_ptr);

    // Write image data row by row
    png_bytep row = malloc(4 * width * sizeof(png_byte)); // Allocate a row buffer
    if (!row) {
        perror("Failed to allocate memory for PNG row");
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        return;
    }

    for (uint32_t y = 0; y < height; y++) {
        memcpy(row, &image_data[y * width * 4], width * 4); // Copy one row at a time
        png_write_row(png_ptr, row);
    }

    // Finish writing the PNG
    png_write_end(png_ptr, NULL);
    free(row);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
}


// Generate particles and simulate movement
void generation_loop(grid_t *grid, uint8_t number_of_particles, uint32_t resolution) {
    printf("[DEBUG] Starting generation loop with %u particles...\n", number_of_particles);
    particle_t *particle_null = malloc(sizeof(particle_t));
    if (!particle_null) {
        perror("Failed to allocate memory for particle_null");
        exit(EXIT_FAILURE);
    }

    dla_init(grid, particle_null, resolution);

    for (int i = 1; i < number_of_particles; i++) {
        particle_t *particle = malloc(sizeof(particle_t));
        if (!particle) {
            perror("Failed to allocate memory for particle");
            exit(EXIT_FAILURE);
        }

        bool particle_done = false;
        particle_init(particle, rand() % grid->resolution, rand() % grid->resolution, grid);
        while (!particle_done) {
            particle_move(grid, particle, &particle_done);
        }

        printf("[DEBUG] Particle %d set\n", i);

        free(particle);
    }

    free(particle_null);
    printf("[DEBUG] Generation loop completed.\n");
}

int main() {
    grid_t grid;
    uint32_t resolution = 128;
    uint8_t particle_num = 40;

    particle_t particle_null = { -1, -1 }; // Initialize a null particle
    particle_t origin_particle = { rand() % resolution, rand() % resolution };

    // Initialize the grid and generate particles
    grid_init(&grid, resolution, origin_particle, particle_null);
    generation_loop(&grid, particle_num, resolution);

    // Generate image data and save it as a PNG file
    uint8_t *image_data = NULL;
    generate_image_data(&grid, &image_data);
    make_png("output.png", image_data, resolution, resolution);

    // Cleanup memory
    for (int i = 0; i < grid.resolution; i++) {
        free(grid.particles[i]);
    }
    free(grid.particles);
    free(image_data); // Free the image data memory

    return 0;
}
