#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <WinSock2.h>

// Particle structure representing each particle
typedef struct {
    int32_t posx, posy;
} particle_t;

// Line structure (not used in this code, can be extended later)
typedef struct {
    bool is_vert;
    particle_t *particles;
    uint32_t startx, starty, endx, endy;
} line_t;

// Grid structure for holding particle positions
typedef struct {
    uint32_t resolution;
    particle_t **particles;
    particle_t particle_null;
} grid_t;

// Function to initialize the grid with empty particles
void grid_init(grid_t *grid, uint32_t resolution, particle_t origin_particle, particle_t particle_null) {
    grid->resolution = resolution;

    // Allocate memory for rows
    grid->particles = malloc(resolution * sizeof(particle_t*));
    
    // Allocate memory for each row and initialize all particles to particle_null
    for (int i = 0; i < resolution; i++) {
        grid->particles[i] = malloc(grid->resolution * sizeof(particle_t));
        for (int j = 0; j < grid->resolution; j++) {
            grid->particles[i][j] = particle_null;
        }
    }

    grid->particle_null = particle_null;

    // Set the origin particle in the grid
    if (origin_particle.posx < resolution && origin_particle.posy < resolution) {
        grid->particles[origin_particle.posx][origin_particle.posy] = origin_particle;
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
}

// Function to initialize the grid and place the origin particle
void dla_init(grid_t *grid, particle_t *particle_null, uint32_t grid_res) {
    srand(time(NULL));
    particle_null->posx = -1;
    particle_null->posy = -1;

    // Allocate and initialize the origin particle
    particle_t origin_particle;
    particle_init(&origin_particle, rand() % grid_res, rand() % grid_res, grid);

    // Initialize the grid
    grid_init(grid, grid_res, origin_particle, *particle_null);
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

// Generate particles and simulate movement
void generation_loop(grid_t *grid, uint8_t number_of_particles, uint32_t resolution) {
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

        free(particle);
    }

    free(particle_null);
}

// Function to write the grid to a PNG file (mockup)
void download_grid(FILE *fptr, char* filename, grid_t *grid) {
    fptr = fopen(filename, "wb");
    if (!fptr) {
        perror("Failed to open file for writing");
        return;
    }

    // PNG signature
    unsigned char png_signature[8] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
    fwrite(png_signature, 1, 8, fptr);

    // IHDR Chunk
    uint8_t ihdr[13];
    uint32_t width = grid->resolution;
    uint32_t height = grid->resolution;
    
    // Write width and height
    ihdr[0] = (width >> 24) & 0xff;
    ihdr[1] = (width >> 16) & 0xff;
    ihdr[2] = (width >> 8) & 0xff;
    ihdr[3] = width & 0xff;
    ihdr[4] = (height >> 24) & 0xff;
    ihdr[5] = (height >> 16) & 0xff;
    ihdr[6] = (height >> 8) & 0xff;
    ihdr[7] = height & 0xff;

    ihdr[8] = 1;  // Bit depth (1 for black/white)
    ihdr[9] = 3;  // Color type (3: indexed color)
    ihdr[10] = 0; // Compression method
    ihdr[11] = 0; // Filter method
    ihdr[12] = 0; // Interlace method (none)

    // Write IHDR chunk
    uint32_t ihdr_len = htonl(13);  // IHDR data length (13 bytes)
    fwrite(&ihdr_len, 4, 1, fptr);  // Write length
    fwrite("IHDR", 1, 4, fptr);     // Write chunk type
    fwrite(ihdr, 1, 13, fptr);      // Write IHDR data

    // PLTE Chunk (black and white palette)
    unsigned char plte[6] = {
        0x00, 0x00, 0x00,  // Black
        0xFF, 0xFF, 0xFF   // White
    };
    uint32_t plte_len = htonl(6);   // Length of PLTE data (6 bytes)
    fwrite(&plte_len, 4, 1, fptr);  // Write length
    fwrite("PLTE", 1, 4, fptr);     // Write chunk type
    fwrite(plte, 1, 6, fptr);       // Write PLTE data

    // IDAT Chunk (image data)
    uint32_t idat_len = htonl(grid->resolution * grid->resolution / 8);  // Length of the image data (for simplicity)
    fwrite(&idat_len, 4, 1, fptr);  // Write length
    fwrite("IDAT", 1, 4, fptr);     // Write chunk type

    // Write image data row by row
    for (int i = 0; i < grid->resolution; i++) {
        uint8_t byte = 0;
        int bit_position = 7;

        for (int j = 0; j < grid->resolution; j++) {
            // If the particle is equal to particle_null, set the pixel to black (0), otherwise white (1)
            uint8_t pixel_value = (grid->particles[i][j].posx == grid->particle_null.posx &&
                                   grid->particles[i][j].posy == grid->particle_null.posy) ? 0 : 1;

            // Set the bit for the current pixel in the byte
            byte |= (pixel_value << bit_position);

            // Move to the next bit
            bit_position--;

            // When we fill up one byte, write it to the file
            if (bit_position < 0) {
                fwrite(&byte, 1, 1, fptr);
                byte = 0;  // Reset the byte
                bit_position = 7;  // Reset the bit position
            }
        }

        // If the row ends and there are remaining bits that haven't been written yet, write them
        if (bit_position != 7) {
            fwrite(&byte, 1, 1, fptr);
        }
    }

    // IEND Chunk
    uint32_t iend_len = 0;
    fwrite(&iend_len, 4, 1, fptr);  // Write length
    fwrite("IEND", 1, 4, fptr);     // Write chunk type

    fclose(fptr);
}

int main() {
    grid_t grid;

    uint8_t particle_num = 40;
    uint32_t resolution = 128;

    generation_loop(&grid, particle_num, resolution);

    // Write the grid to a PNG file
    download_grid(NULL, "output.png", &grid);

    // Cleanup memory
    for (int i = 0; i < grid.resolution; i++) {
        free(grid.particles[i]);  // Free each row
    }
    free(grid.particles);  // Free the row pointers

    return 0;
}
