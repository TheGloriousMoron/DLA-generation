#include "defs.h"

void grid_constructor(grid_t *grid, uint32_t size, uint32_t particle_num) {
    // Allocate memory for log
    log_t *log = (log_t*)malloc(sizeof(log_t));

#ifdef _WIN32
    char *filename = "C:\\Users\\james\\source\\repos\\DLA-generation\\out\\log\\grid_constructor.txt";
#else
    char *filename = "/workspaces/DLA-generation/out/log/grid_constructor.txt";
#endif

    log_constructor(log, filename);  // Initialize log

    // Log grid size and particle number
    log_write(log, "[DEBUG] Initializing grid constructor with size: %u and particle_num: %u\n", size, particle_num);

    // Check if grid size is valid
    if (size <= 0) {
        log_write(log, "[ERROR] Invalid grid size: %u\n", size);
        log_deconstructor(log);
        return;  // Exit early to avoid further issues
    }

    grid->grid_size = size;
    grid->particle_num = particle_num;
    grid->particle_empty = Vector2_int(-1, -1); // "null" particle indicator
    grid->particle_adjecent = Vector2_int(-2, -2);

    // Allocate memory for the particle array (columns)
    log_write(log, "[DEBUG] Allocating memory for particle array\n");
    grid->particle_array = malloc(sizeof(vector2_t*) * grid->grid_size);

    // Check if memory allocation for particle_array succeeded
    if (grid->particle_array == NULL) {
        log_write(log, "[ERROR] Failed to allocate memory for particle array (columns)\n");
        log_deconstructor(log);
        return;  // Exit if memory allocation fails
    }

    // Allocate memory for each row in the particle array
    for (int x = 0; x < grid->grid_size; x++) {
        log_write(log, "[DEBUG] Allocating memory for particle row %d\n", x);
        grid->particle_array[x] = malloc(sizeof(vector2_t) * grid->grid_size);

        // Check if memory allocation for each row succeeded
        if (grid->particle_array[x] == NULL) {
            log_write(log, "[ERROR] Failed to allocate memory for particle row %d\n", x);

            // Free already allocated memory if we encounter an error
            for (int i = 0; i < x; i++) {
                free(grid->particle_array[i]);
                log_write(log, "[DEBUG] Freed row %d\n", i);
            }

            free(grid->particle_array);
            log_write(log, "[DEBUG] Freed particle array (columns)\n");

            log_deconstructor(log);

            return;  // Exit if memory allocation fails
        }
    }

    // Set all particles in the array to particle_empty
    for (int x = 0; x < grid->grid_size; x++) {
        for (int y = 0; y < grid->grid_size; y++) {
            grid->particle_array[x][y] = grid->particle_empty;
            log_write(log, "[DEBUG] Set particle_array[%d][%d] to particle_empty\n", x, y);
        }
    }

    // Set the origin particle
    vector2_t origin_particle;
    int32_t r1 = rand() % grid->grid_size;
    int32_t r2 = rand() % grid->grid_size;
    origin_particle = Vector2_int(r1, r2);
    log_write(log, "[DEBUG] Origin particle created at %d, %d\n", origin_particle.x, origin_particle.y);

    // Set the origin particle in the grid
    grid_set_particle(grid, &origin_particle);
    log_write(log, "[DEBUG] Origin particle set in grid\n");

    log_write(log, "[DEBUG] Grid successfully constructed\n");

    // Deconstruct log
    log_deconstructor(log);
}

void grid_set_particle(grid_t *grid, vector2_t *p) {
    log_t *log = (log_t*)malloc(sizeof(log_t));

    // Platform-specific logging path
#ifdef _WIN32
    char *filename = "C:\\Users\\james\\source\\repos\\DLA-generation\\out\\log\\grid_set_particle.txt";
#else
    char *filename = "/workspaces/DLA-generation/out/log/grid_set_particle.txt";
#endif

    log_constructor(log, filename);

    // Check if particle position is within bounds
    if (p->x < 0 || p->x >= grid->grid_size || p->y < 0 || p->y >= grid->grid_size) {
        log_write(log, "[ERROR] Invalid particle position at %d, %d\n", p->x, p->y);
        log_deconstructor(log);
        return;
    }

    // Set adjacent particles if they are empty
    for (int x = -1; x < 3; x += 2) {
        for (int y = -1; y < 3; y += 2) {
            int new_x = p->x + x;
            int new_y = p->y + y;
            // Check if the new coordinates are within grid bounds
            if (new_x >= 0 && new_x < grid->grid_size && new_y >= 0 && new_y < grid->grid_size) {
                if (Vector2_is_equal(grid->particle_array[new_x][new_y], grid->particle_empty)) {
                    grid->particle_array[new_x][new_y] = grid->particle_adjecent;
                }
            }
        }
    }

    // Set the particle at its position
    grid->particle_array[p->x][p->y] = Vector2_int(p->x, p->y);

    log_write(log, "[DEBUG] Particle set at position: %d, %d\n", p->x, p->y);

    // Deconstruct log
    log_deconstructor(log);
}

bool grid_check_particle(grid_t *grid, vector2_t *p) {
    log_t *log = (log_t*)malloc(sizeof(log_t));

    // Platform-specific logging path
#ifdef _WIN32
    char *filename = "C:\\Users\\james\\source\\repos\\DLA-generation\\out\\log\\grid_check_particle.txt";
#else
    char *filename = "/workspaces/DLA-generation/out/log/grid_check_particle.txt";
#endif

    log_constructor(log, filename);

    // Check if particle position is within bounds
    if (p->x < 0 || p->x >= grid->grid_size || p->y < 0 || p->y >= grid->grid_size) {
        log_write(log, "[ERROR] Invalid particle position at %d, %d\n", p->x, p->y);
        log_deconstructor(log);
        return false;
    }

    log_write(log, "[DEBUG] Checking particle at position: %d, %d\n", p->x, p->y);

    bool result = Vector2_is_equal(grid->particle_array[p->x][p->y], grid->particle_adjecent);
    
    log_write(log, "[DEBUG] Particle check result: %s\n", result ? "true" : "false");

    // Deconstruct log
    log_deconstructor(log);

    return result;
}

void grid_active(grid_t *grid) {
    log_t *log = (log_t*)malloc(sizeof(log_t));

    // Platform-specific logging path
#ifdef _WIN32
    char *filename = "C:\\Users\\james\\source\\repos\\DLA-generation\\out\\log\\grid_active.txt";
#else
    char *filename = "/workspaces/DLA-generation/out/log/grid_active.txt";
#endif

    log_constructor(log, filename);
    log_write(log, "[DEBUG] Started grid active\n");

    // Simulate particle movement
    for (int i = 0; i < grid->particle_num; i++) {
        log_write(log, "Loop %d\n", i);
        bool is_done = false;
        bool is_stuck = true;

        // Generate random start position
        int32_t r1 = rand() % grid->grid_size;
        int32_t r2 = rand() % grid->grid_size;
        vector2_t p = Vector2_int(r1, r2); // Random start position
        log_write(log, "[DEBUG] Particle %d created at: %d, %d\n", i, r1, r2);

        // Check if the starting position is valid (i.e., empty)
        while (is_stuck) {
            log_write(log, "[DEBUG] Particle %d is stuck\n", i);
            if (Vector2_is_equal(grid->particle_array[p.x][p.y], grid->particle_empty)) {
                is_stuck = false; // Start position is empty
                log_write(log, "[DEBUG] Particle %d is not stuck anymore\n", i);
            } else {
                // Generate a new random position if the current one is occupied
                r1 = rand() % grid->grid_size;
                r2 = rand() % grid->grid_size;
                p = Vector2_int(r1, r2);
                log_write(log, "[DEBUG] Particle %d is now at: %d, %d\n", i, r1, r2);
            }
        }

        // Randomly move the particle until it "sticks" to an occupied space
        while (!is_done) {
            log_write(log, "[DEBUG] Particle %d is entering movement loop\n", i);
            uint8_t move_dir = rand() % 4;

            if (move_dir == 0 && p.x + 1 < grid->grid_size) {  // Move right
                vector2_t _p = Vector2_int(p.x + 1, p.y);
                if (grid_check_particle(grid, &_p)) {
                    grid_set_particle(grid, &_p);
                    is_done = true;
                } else {
                    p.x += 1;
                }
            } else if (move_dir == 1 && p.x - 1 >= 0) {  // Move left
                vector2_t _p = Vector2_int(p.x - 1, p.y);
                if (grid_check_particle(grid, &_p)) {
                    grid_set_particle(grid, &_p);
                    is_done = true;
                } else {
                    p.x -= 1;
                }
            } else if (move_dir == 2 && p.y + 1 < grid->grid_size) {  // Move down
                vector2_t _p = Vector2_int(p.x, p.y + 1);
                if (grid_check_particle(grid, &_p)) {
                    grid_set_particle(grid, &_p);
                    is_done = true;
                } else {
                    p.y += 1;
                }
            } else if (move_dir == 3 && p.y - 1 >= 0) {  // Move up
                vector2_t _p = Vector2_int(p.x, p.y - 1);
                if (grid_check_particle(grid, &_p)) {
                    grid_set_particle(grid, &_p);
                    is_done = true;
                } else {
                    p.y -= 1;
                }
            }
        }
    }

    // Deconstruct log
    log_write(log, "[DEBUG] Finished grid active\n");
    log_deconstructor(log);
}

void grid_deconstructor(grid_t *grid) {
    log_t *log = (log_t*)malloc(sizeof(log_t));

    // Platform-specific logging path
#ifdef _WIN32
    char *filename = "C:\\Users\\james\\source\\repos\\DLA-generation\\out\\log\\grid_deconstructor.txt";
#else
    char *filename = "/workspaces/DLA-generation/out/log/grid_deconstructor.txt";
#endif

    log_constructor(log, filename);  // Initialize log

    // Free each row in the array
    for (int x = 0; x < grid->grid_size; x++) {
        log_write(log, "[DEBUG] Freeing particle row %d\n", x);
        free(grid->particle_array[x]);
        log_write(log, "[DEBUG] Freed particle row %d\n", x);
    }

    log_write(log, "[DEBUG] Freeing particle array columns\n");
    free(grid->particle_array);
    log_write(log, "[DEBUG] Freed particle array\n");

    log_write(log, "[DEBUG] Freeing grid\n");
    free(grid);
    log_write(log, "[DEBUG] Grid freed\n");

    // Deconstruct log
    log_deconstructor(log);
}