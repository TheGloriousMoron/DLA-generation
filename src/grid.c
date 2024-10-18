#include "grid.h"

const rgba_t white = {255, 255, 255, 255};
const rgba_t black = {0, 0, 0, 255};
const rgba_t particle_color = {255, 0, 255, 255};

void grid_alloc(grid_t *grid, uint32_t size, float c) {
    grid->size = size;

    if (c >= 1.0f) {
        printf(LOG_TYPE_WARNING);
        printf(LOG_WARNING_GRID_0);
        char m = (char)getchar();
        if (tolower(m) == 'y') {  
            printf(LOG_TYPE_INFO);
            printf(LOG_INFO_MSG_0);
            c = 0.5f;
        } else {
            printf(LOG_TYPE_INFO);
            printf(LOG_INFO_MSG_1);
            exit(0);
        }
    }

    grid->particle_max = (uint32_t)((grid->size * grid->size) * c);

    // Allocate memory for weights as a 2D array
    grid->weights = (float**)malloc(sizeof(float*) * grid->size);
    if (!grid->weights) {
        printf(LOG_TYPE_ERROR);
        printf(LOG_ERROR_GRID_0);
        exit(-1);
    }

    for (int i = 0; i < grid->size; i++) {
        grid->weights[i] = (float*)malloc(sizeof(float) * grid->size);
        if (!grid->weights[i]) {
            printf(LOG_TYPE_ERROR);
            printf(LOG_ERROR_GRID_1);
            exit(-1);
        }
    }

    // Allocate memory for the particle array
    grid->particles = (vector_t*)malloc(sizeof(vector_t) * grid->particle_max);
    if (!grid->particles) {
        printf(LOG_TYPE_ERROR);
        printf(LOG_ERROR_GRID_2);
        exit(-1);
    }

    printf(LOG_TYPE_SUCCESS);
    printf(LOG_SUCCESS_GRID_0);
}

void grid_init(grid_t *grid, vector_t *start_particles, uint32_t start_particle_count, vector_t *start_weight_pos, float* start_weight_val, uint32_t start_weight_count) {
    // Error if too many starting particles
    if (start_particle_count >= grid->particle_max) {
        printf(LOG_TYPE_ERROR);
        printf(LOG_ERROR_GRID_3);
        exit(-1);
    }
    // Error if too many weights
    if (start_weight_count > grid->size * grid->size) {
        printf(LOG_TYPE_ERROR);
        printf(LOG_ERROR_GRID_4);
        exit(-1);
    }

    grid->particle_count = start_particle_count;
    
    // Set weights
    for (int i = 0; i < start_weight_count; i++) {
        grid->weights[start_weight_pos[i].x][start_weight_pos[i].y] = start_weight_val[i];
    }

    // Set particles
    for (int i = 0; i < start_particle_count; i++) {
        grid->particles[i] = start_particles[i];
        grid->weights[grid->particles[i].x][grid->particles[i].y] = 0.0f; // Set particle's weight to 0
    }

    printf(LOG_TYPE_SUCCESS);
    printf("Initialized grid with a size of %d and a particle number of %d\n", grid->size, grid->particle_max);
}

bool is_occupied(grid_t *grid, int x, int y) {
    for (int j = 0; j < grid->particle_count; j++) {
        if (grid->particles[j].x == x && grid->particles[j].y == y) {
            return true;
        }
    }
    return false;
}

void move_particle(grid_t *grid, vector_t *particle) {
    bool valid_move = false;
    while (!valid_move) {
        int move_dir = rand() % 4;

        switch (move_dir) {
            case (0): // Move Left
                if (particle->x != 0) {
                    particle->x -= 1;
                    valid_move = true;
                }
                break; // Fixed: Added break
            case (1): // Move Right
                if (particle->x < grid->size - 1) {
                    particle->x += 1;
                    valid_move = true;
                }
                break; // Fixed: Added break
            case (2): // Move Up
                if (particle->y != 0) {
                    particle->y -= 1;
                    valid_move = true;
                }
                break; // Fixed: Added break
            case (3): // Move Down
                if (particle->y < grid->size - 1) {
                    particle->y += 1;
                    valid_move = true;
                }
                break; // Fixed: Added break
            default:
                break;
        }
    }    
}

// Function to simulate particles' movements
void grid_simulate_particles(grid_t *grid) {
    for (int i = grid->particle_count; i < grid->particle_max; i++) {
        printf(LOG_TYPE_INFO);
        printf("Beginning simulation for particle %d out of %d.\n", i, grid->particle_max);
        
        vector_t *particle = (vector_t*)malloc(sizeof(vector_t));
        bool starting_spot_found = false;

        // Generate a starting position for the new particle
        while (!starting_spot_found) {
            particle->x = rand() % grid->size;
            particle->y = rand() % grid->size;

            if (!is_occupied(grid, particle->x, particle->y)) {
                starting_spot_found = true;
            }
        }

        bool is_resting = false;
        while (!is_resting) {
            // Check if the particle is resting (surrounded by others)
            if (is_occupied(grid, particle->x - 1, particle->y) || 
                is_occupied(grid, particle->x + 1, particle->y) ||
                is_occupied(grid, particle->x, particle->y - 1) || 
                is_occupied(grid, particle->x, particle->y + 1)) {
                is_resting = true; // Corrected logic: set is_resting to true
                break; // Particle is resting
            }

            // Move the particle
            move_particle(grid, particle);
        }

        // Save particle's position after movement
        grid->particles[grid->particle_count] = *particle; // Store new particle position
        grid->particle_count++; // Increment particle count
        free(particle); // Free allocated memory for the particle
    }
}

// Function to output a 2D array
rgba_t** grid_get_simulation_data(grid_t *grid) {
    // Loop through all of the particles and set the weight to 2.0
    for (int i = 0; i < grid->particle_count; i++) {
        grid->weights[grid->particles[i].x][grid->particles[i].y] = 2.0f;
    }

    // Create the return array and the colors
    rgba_t **data;
    data = (rgba_t**)malloc(sizeof(rgba_t*) * grid->size);
    for (int i = 0; i < grid->size; i++) {
        data[i] = (rgba_t*)malloc(sizeof(rgba_t) * grid->size);
    }

    // Loop through filling the data;
    for (int c = 0; c < grid->size; c++) {
        for (int r = 0; r < grid->size; r++) {
            // Assign the color to white
            data[c][r] = white;
            data[c][r].a *= grid->weights[c][r];
        }
    }
    
    // Loop through filling the particle data
    for (int i = 0; i < grid->particle_count; i++) {
        data[grid->particles[i].x][grid->particles[i].y] = particle_color;
    }

    return data;
}

void grid_free(grid_t *grid) {
    if (grid->particles) {
        free(grid->particles);
    }
    for (int i = 0; i < grid->size; i++) {
        if (grid->weights[i]) {
            free(grid->weights[i]);
        }
    }
    if (grid->weights) {
        free(grid->weights);
    }

    free(grid);
}

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

void load_grid_file(char *name, char *data) {
    size_t filename_len = strlen(GRIDPATH) + strlen(name) + 1; // +1 for the null terminator
    char *filename = (char*)malloc(filename_len);

    strcpy(filename, GRIDPATH);
    strcat(filename, name);

    FILE *fp = fopen(filename, "r");
    // Get the length of the file
    fseek(fp, 0, SEEK_END);
    size_t file_size = ftell(fp);
    rewind(fp);

    data = (char*)malloc(file_size + 1);
    fread(data, 1, file_size, fp);
    data[file_size] = '\0';
}

void convert_grid_data(char *data, uint32_t *size, uint32_t *particle_count, vector_t *positions) {
    char *line = NULL;
    char *data_buffer = strdup(data); // Duplicate data for safe manipulation
    char *rest = data_buffer;  // Pointer to iterate through the buffer

    // Get the first argument (grid size)
    line = strtok_r(rest, "\n", &rest);  // Extract first line
    if (line != NULL) {
        *size = (uint32_t)atoi(line);  // Assign grid size
    }

    // Get the second argument (particle count)
    line = strtok_r(rest, "\n", &rest);  // Extract second line
    if (line != NULL) {
        *particle_count = (uint32_t)atoi(line);  // Assign particle count
    }

    // Allocate memory for the positions array based on particle count
    positions = (vector_t*)malloc(sizeof(vector_t) * (*particle_count));
    if (positions == NULL) {
        free(data_buffer);
        exit(-1);
    }

    // Parse particle positions (one per line)
    for (uint32_t i = 0; i < *particle_count; i++) {
        line = strtok_r(rest, "\n", &rest);  // Extract next line
        if (line == NULL) {
            exit(-1);
        }

        // Split the line into two tokens: x and y
        char *x_string = strtok(line, " ");
        char *y_string = strtok(NULL, " ");

        if (x_string != NULL && y_string != NULL) {
            positions[i].x = atoi(x_string);
            positions[i].y = atoi(y_string);
        } else {
            exit(-1);
        }
    }

    // Clean up
    free(data_buffer);
}