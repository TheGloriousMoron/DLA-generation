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
    if (start_particle_count >= grid->particle_max) {
        printf(LOG_TYPE_ERROR);
        printf(LOG_ERROR_GRID_3);
        exit(-1);
    }
    if (start_weight_count > grid->size * grid->size) {
        printf(LOG_TYPE_ERROR);
        printf(LOG_ERROR_GRID_4);
        exit(-1);
    }

    grid->particle_count = start_particle_count; // Set particle count correctly
    printf(LOG_TYPE_INFO);
    printf("Starting particle count: %d, Maximum particle count: %d\n", grid->particle_count, grid->particle_max);

    // Set weights
    for (int i = 0; i < start_weight_count; i++) {
        grid->weights[start_weight_pos[i].x][start_weight_pos[i].y] = start_weight_val[i];
    }

    // Set particles
    for (int i = 0; i < start_particle_count; i++) {
        grid->particles[i].x = start_particles[i].x; // Store new particle position
        grid->particles[i].y = start_particles[i].y;
    }

    printf(LOG_TYPE_SUCCESS);
    printf("Initialized grid with a size of %d and a particle number of %d\n", grid->size, grid->particle_count);
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
    if (filename == NULL) {
        printf(LOG_TYPE_ERROR);
        printf("Failed to allocate memory for the grid filename.\n");
        exit(-1);
    } else {
        printf(LOG_TYPE_SUCCESS);
        printf("Succesfully allocated memory for grid filename.\n");
    }

    strcpy(filename, GRIDPATH);
    strcat(filename, name);

    printf(LOG_TYPE_INFO);
    printf("Grid filepath: %s\n", filename);

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf(LOG_TYPE_ERROR);
        printf("Failed to open grid file.\n");
        exit(-1);
    } else {
        printf(LOG_TYPE_SUCCESS);
        printf("Success to open grid file.\n");
    }
    // Get the length of the file
    fseek(fp, 0, SEEK_END);
    size_t file_size = ftell(fp);
    rewind(fp);

    data = (char*)malloc(file_size + 1);
    if (data == NULL) {
        printf(LOG_TYPE_ERROR);
        printf("Failed to allocate memory for the data array.\n");
        exit(-1);
    } else {
        printf(LOG_TYPE_SUCCESS);
        printf("Success to allocate memory for the data array.\n");
    }
    fread(data, 1, file_size, fp);
    data[file_size] = '\0';
    printf(LOG_TYPE_SUCCESS);
    printf("File data created succesfully.\n");
    printf(LOG_TYPE_INFO);
    printf("Would you like to see the data?\n[y/n]\n");
    char input = getchar();
    if (input == 'y') {
        printf("\n %s",LOG_TYPE_INFO);
        printf("File data: %s", data);
        printf("\n");
    }
}

void convert_grid_data(char *raw_data, uint32_t *size, uint32_t *particle_count, vector_t **positions) {
    printf(LOG_TYPE_INFO);
    printf("Entering convert_grid_data function\n");

    char **tokens = NULL;

    *tokens = strtok(raw_data, " \n");
    int token_count = 0;
    while (tokens[token_count] != '\0') {
        token_count++;
    }

    // Initialize variables to track parsing
    bool parsing_vectors = false;
    (*particle_count) = 0;
    int particle_index = 0;

    for (int i = 0; i < token_count; i++) {
        int token_length = strlen(tokens[i]);
        if (tokens[i][0] == '@') {
            switch(tokens[i][1]) {
                case 'r':
                    // Index i by one to go to the next token
                    i++;
                    (*size) = atoi(tokens[i]);
                    printf(LOG_TYPE_INFO);
                    printf("Resolution comes from token %d, value of %d\n", i, (*size));
                    break;
                case 'c':
                    // Index i by one to go to the next token
                    i++;
                    (*particle_count) = atoi(tokens[i]);
                    (*positions) = (vector_t*)malloc(sizeof(vector_t) * (*particle_count));
                    printf(LOG_TYPE_INFO);
                    printf("Particle Count comes from token %d, value of %d\n", i, (*particle_count));
                    break;
                case 'v':
                    // Index i by one to go to the next token
                    i++;
                    vector_t v;
                    v.x = atoi(tokens[i]);
                    // Index i by one again to get the y value
                    i++;
                    v.y = atoi(tokens[i]);
                    (*positions) = (vector_t*)malloc(sizeof(vector_t) * (*particle_count));
                    (*positions)[particle_index] = v;
                    particle_index++;
                    printf(LOG_TYPE_INFO);
                    printf("Particle %d comes from tokens %d x %d y %d is set to values x %d y %d\n", particle_index, i - 2, i - 1, i, v.x, v.y);
                    break;
                default:
                    break;
            }
        }
    }

    for (int i = 0; i < token_count; i++) {
        free(tokens[i]);
    }
    free(tokens);
    
    printf(LOG_TYPE_INFO);
    printf("Finished parsing grid data.\n");
    printf(LOG_TYPE_INFO);
    printf("Resolution: %d\n", *size);
    printf(LOG_TYPE_INFO);
    printf("Particle count: %d\n", *particle_count);
    for (size_t i = 0; i < *particle_count; i++) {
        printf(LOG_TYPE_INFO);
        printf("Particle %zu is at position: (%d, %d)\n", i, (*positions)[i].x, (*positions)[i].y);
    }
}
