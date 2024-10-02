#include "defs.h"


void grid_constructor(grid_t *grid, uint32_t size, uint32_t particle_num) {
    // Allocate memory for log
    log_t *log = (log_t*)malloc(sizeof(log_t));
    char *filename = "C:\\Users\\james\\source\\repos\\DLA-generation\\out\\log\\grid_constructor.txt";
    log_constructor(log, filename);  // Initialize log

    grid->grid_size = size;
    grid->particle_num = particle_num;
    grid->particle_empty = Vector2_int(-1, -1); // Use a "null" particle as empty indicator
    grid->particle_adjecent = Vector2_int(-2, -2);

    // Allocate memory for the particle array
    log_write(log, "[DEBUG] Creating particle array columns\n");
    grid->particle_array = malloc(sizeof(vector2_t*) * grid->grid_size); // Allocate memory for each column of the grid

    for (int x = 0; x < grid->grid_size; x++) {
        log_write(log, "[DEBUG] Creating particle array row for column %d\n", x);
        grid->particle_array[x] = malloc(sizeof(vector2_t) * grid->grid_size);
    }

    // Set all of the particles to particle_empty
    for (int x = 0; x < grid->grid_size; x++) {
        for (int y = 0; y < grid->grid_size; y++) {
            log_write(log, "[DEBUG] Setting particle array %d, %d to particle empty\n", x, y);
            grid->particle_array[x][y] = grid->particle_empty;
        }
    }

    // Set the origin particle
    vector2_t origin_particle;
    int32_t r1 = rand() % grid->grid_size; 
    int32_t r2 = rand() % grid->grid_size;
    origin_particle = Vector2_int(r1, r2);
    log_write(log, "[DEBUG] Origin particle created at %d, %d\n", origin_particle.x, origin_particle.y);
    grid_set_particle(grid, &origin_particle);
    log_write(log, "[DEBUG] Origin particle set\n");

    log_write(log, "[DEBUG] Grid successfully constructed\n");

    // Deconstruct log
    log_deconstructor(log);
    free(log);  // Free allocated memory for log
}

void grid_set_particle(grid_t *grid, vector2_t *p) {
    for (int x = -1; x < 3; x+=2) {
        for (int y = -1; y < 3; y+=2) {
            if (Vector2_is_equal(grid->particle_array[p->x + x][p->y + y], grid->particle_empty)) {
                grid->particle_array[p->x + x][p->y + y] = grid->particle_adjecent;
            }
        }
    }
    grid->particle_array[p->x][p->y] = Vector2_int(p->x, p->y);
}

bool grid_check_particle(grid_t *grid, vector2_t *p) {
    if (Vector2_is_equal(grid->particle_array[p->x][p->y], grid->particle_adjecent)) {
        return true;
    } else {
        return false;
    }
}

void grid_active(grid_t *grid) {
    // Allocate memory for log
    log_t *log = (log_t*)malloc(sizeof(log_t));
    char *filename = "C:\\Users\\james\\source\\repos\\DLA-generation\\out\\log\\grid_active.txt";
    log_constructor(log, filename);  // Initialize log

    log_write(log, "[DEBUG] Started grid active\n");

    // Simulate particle movement
    for (int i = 0; i < grid->particle_num; i++) {
        log_write(log, "Loop %d\n", i);
        bool is_done = false;
        bool is_stuck = true;

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
            uint8_t move_dir = rand() % 4; // Generate a random direction (0-3)
            log_write(log, "[DEBUG] Particle %d's move direction is %d\n", i, move_dir);

            if (move_dir == 0) {

            } else if (move_dir == 1) {
                
            } else if (move_dir == 2) {

            } else if (move_dir == 3) {

            }
        }
    }

    // Deconstruct log
    log_deconstructor(log);
    free(log);  // Free allocated memory for log
}

void grid_deconstructor(grid_t *grid) {
    // Allocate memory for log
    log_t *log = (log_t*)malloc(sizeof(log_t));
    char *filename = "C:\\Users\\james\\source\\repos\\DLA-generation\\out\\log\\grid_deconstructor.txt";
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
    free(log);  // Free allocated memory for log
}
