#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <winsock2.h>

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

// CRC32 lookup table
uint32_t crc32_table[256];

// Function to generate the CRC32 lookup table
void generate_crc32_table() {
    printf("[DEBUG] Generating CRC32 table...\n");
    uint32_t crc;
    for (uint32_t i = 0; i < 256; i++) {
        crc = i;
        for (uint32_t j = 0; j < 8; j++) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc >>= 1;
        }
        crc32_table[i] = crc;
    }
    printf("[DEBUG] CRC32 table generated successfully.\n");
}

// Manually implemented CRC32 function
uint32_t crc32(uint32_t crc, const unsigned char *buf, size_t len) {
    crc ^= ~0U;
    while (len--) {
        crc = crc32_table[(crc ^ *buf++) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ ~0U;
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

        printf("[DEBUG] Paricle %d set", i);

        free(particle);
    }

    free(particle_null);
    printf("[DEBUG] Generation loop completed.\n");
}

// Function to write a PNG image filled entirely with white pixels
void write_png_white_image(const char *filename, uint32_t resolution) {
    FILE *fptr = fopen(filename, "wb");
    if (!fptr) {
        perror("Failed to open file for writing");
        return;
    }

    // PNG signature
    unsigned char png_signature[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    fwrite(png_signature, 1, 8, fptr);

    // IHDR Chunk (Image Header)
    uint8_t ihdr[13];
    ihdr[0] = (resolution >> 24) & 0xff;
    ihdr[1] = (resolution >> 16) & 0xff;
    ihdr[2] = (resolution >> 8) & 0xff;
    ihdr[3] = resolution & 0xff;
    ihdr[4] = (resolution >> 24) & 0xff;
    ihdr[5] = (resolution >> 16) & 0xff;
    ihdr[6] = (resolution >> 8) & 0xff;
    ihdr[7] = resolution & 0xff;
    ihdr[8] = 1;  // Bit depth (1 for black/white)
    ihdr[9] = 3;  // Color type (3: indexed color)
    ihdr[10] = 0; // Compression method
    ihdr[11] = 0; // Filter method
    ihdr[12] = 0; // Interlace method (none)

    uint32_t ihdr_len = htonl(13);  // IHDR data length (13 bytes)
    fwrite(&ihdr_len, 4, 1, fptr);  // Write length
    fwrite("IHDR", 1, 4, fptr);     // Write chunk type
    fwrite(ihdr, 1, 13, fptr);      // Write IHDR data
    uint32_t crc_ihdr = crc32(0L, Z_NULL, 0);  // Initialize CRC
    crc_ihdr = crc32(crc_ihdr, (const unsigned char *)"IHDR", 4);  // Update CRC with chunk type
    crc_ihdr = crc32(crc_ihdr, ihdr, 13);  // Update CRC with IHDR data
    uint32_t crc_ihdr_final = htonl(crc_ihdr);
    fwrite(&crc_ihdr_final, 4, 1, fptr);

    // PLTE Chunk (Palette: black and white colors)
    unsigned char plte[6] = {
        0x00, 0x00, 0x00,  // Black
        0xFF, 0xFF, 0xFF   // White
    };
    uint32_t plte_len = htonl(6);   // Length of PLTE data (6 bytes)
    fwrite(&plte_len, 4, 1, fptr);  // Write length
    fwrite("PLTE", 1, 4, fptr);     // Write chunk type
    fwrite(plte, 1, 6, fptr);       // Write PLTE data
    uint32_t crc_plte = crc32(0L, Z_NULL, 0);  // Initialize CRC
    crc_plte = crc32(crc_plte, (const unsigned char *)"PLTE", 4);  // Update CRC with chunk type
    crc_plte = crc32(crc_plte, plte, 6);  // Update CRC with PLTE data
    uint32_t crc_plte_final = htonl(crc_plte);
    fwrite(&crc_plte_final, 4, 1, fptr);

    // IDAT Chunk (Image Data - filled with white pixels)
    uint32_t image_data_size = (resolution / 8 + 1) * resolution;  // Include filter bytes
    uint32_t idat_len = htonl(image_data_size);
    fwrite(&idat_len, 4, 1, fptr);  // Write length
    fwrite("IDAT", 1, 4, fptr);     // Write chunk type

    // Compress the image data using zlib
    unsigned char *image_data = (unsigned char *)malloc(image_data_size);
    if (!image_data) {
        perror("Failed to allocate memory for image data");
        fclose(fptr);
        return;
    }

    for (uint32_t i = 0; i < resolution; i++) {
        image_data[i * (resolution / 8 + 1)] = 0;  // Filter byte for each scanline
        for (uint32_t j = 0; j < resolution / 8; j++) {
            image_data[i * (resolution / 8 + 1) + j + 1] = 0xFF;  // All white pixels
        }
    }

    uLongf compressed_size = compressBound(image_data_size);
    unsigned char *compressed_data = (unsigned char *)malloc(compressed_size);
    if (compress(compressed_data, &compressed_size, image_data, image_data_size) != Z_OK) {
        perror("Failed to compress image data");
        free(image_data);
        fclose(fptr);
        return;
    }

    fwrite(compressed_data, 1, compressed_size, fptr);  // Write compressed IDAT data
    free(image_data);
    free(compressed_data);

    uint32_t crc_idat = crc32(0L, Z_NULL, 0);  // Initialize CRC
    crc_idat = crc32(crc_idat, (const unsigned char *)"IDAT", 4);  // Update CRC with chunk type
    crc_idat = crc32(crc_idat, compressed_data, compressed_size);  // Update CRC with compressed data
    uint32_t crc_idat_final = htonl(crc_idat);
    fwrite(&crc_idat_final, 4, 1, fptr);

    // IEND Chunk (End of the image)
    uint32_t iend_len = 0;
    fwrite(&iend_len, 4, 1, fptr);  // Write length (0)
    fwrite("IEND", 1, 4, fptr);     // Write chunk type
    uint32_t crc_iend = crc32(0L, Z_NULL, 0);  // Initialize CRC
    crc_iend = crc32(crc_iend, (const unsigned char *)"IEND", 4);  // Update CRC with chunk type
    uint32_t crc_iend_final = htonl(crc_iend);
    fwrite(&crc_iend_final, 4, 1, fptr);

    fclose(fptr);
    printf("PNG file '%s' written successfully.\n", filename);
}

// Function to write the grid to a PNG file (mockup)
void download_grid(const char* filename, grid_t *grid) {
    printf("[DEBUG] Writing grid to file: %s\n", filename);
    FILE *fptr = fopen(filename, "wb");
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
    uint32_t crc_ihdr = crc32(0, (unsigned char*)"IHDR", 4);
    crc_ihdr = crc32(crc_ihdr, ihdr, 13);
    fwrite(&crc_ihdr, 4, 1, fptr);

    // PLTE Chunk (black and white palette)
    unsigned char plte[6] = {
        0x00, 0x00, 0x00,  // Black
        0xFF, 0xFF, 0xFF   // White
    };
    uint32_t plte_len = htonl(6);   // Length of PLTE data (6 bytes)
    fwrite(&plte_len, 4, 1, fptr);  // Write length
    fwrite("PLTE", 1, 4, fptr);     // Write chunk type
    fwrite(plte, 1, 6, fptr);       // Write PLTE data
    uint32_t crc_plte = crc32(0, (unsigned char*)"PLTE", 4);
    crc_plte = crc32(crc_plte, plte, 6);
    fwrite(&crc_plte, 4, 1, fptr);

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
    uint32_t crc_iend = crc32(0, (unsigned char*)"IEND", 4);
    fwrite(&crc_iend, 4, 1, fptr);

    fclose(fptr);
    printf("[DEBUG] Grid written to PNG file successfully.\n");
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed.\n");
        return 1;
    }

    // Generate the CRC32 lookup table
    generate_crc32_table();

    grid_t grid;

    uint8_t particle_num = 40;
    uint32_t resolution = 128;

    // Start the generation loop
    generation_loop(&grid, particle_num, resolution);

    // Write the grid to a PNG file
    download_grid("test1.png", &grid);

    // Cleanup memory
    for (int i = 0; i < grid.resolution; i++) {
        free(grid.particles[i]);  // Free each row
    }
    free(grid.particles);  // Free the row pointers

    // Cleanup Winsock
    WSACleanup();
    printf("[DEBUG] Program completed successfully.\n");
    return 0;
}
