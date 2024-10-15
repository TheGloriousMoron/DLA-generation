#include "defs.h"

grid_t *grid_init(arguments_t* args)
{
    grid_t *grid = (grid_t *)malloc(sizeof(grid_t));
    // error handling if grid isn't initialized
    if (!grid)
    {
        perror("[ERROR] Grid couldn't be created\n");
        exit(-1);
    }

    // set the particle states
    grid->fill_state = 1;
    grid->adjacent_state = 0;
    grid->empty_state = -1;

    // set the simulation variables
    grid->max_particles = args->particle_count * args->grid_size;
    grid->current_particles = 0;
    grid->size = args->grid_size;
    grid->iteration_max = 256;

    // initialize the grid array
    grid->array = malloc(grid->size * sizeof(int32_t *)); // Allocate memory for the rows
    if (!grid->array)
    {
        perror("[ERROR] Grid array couldn't be created\n");
        exit(-1);
    }
    for (int i = 0; i < grid->size; i++)
    {
        grid->array[i] = malloc(grid->size * sizeof(int32_t)); // Allocate memory for each column
        if (!grid->array[i])
        {
            perror("[ERROR] Grid array row couldn't be created\n");
            exit(-1);
        }
    }

    for (int x = 0; x < grid->size; x++)
    {
        for (int y = 0; y < grid->size; y++)
        {
            grid->array[x][y] = grid->empty_state;
        }
    }

    return grid;
}

bool check_particle_inside(grid_t *grid, int32_t *pos)
{
    return pos[0] >= 0 && pos[0] < grid->size && pos[1] >= 0 && pos[1] < grid->size;
}

bool check_particle_adjecent(grid_t *grid, int32_t *pos)
{
    return grid->array[pos[0]][pos[1]] == grid->adjacent_state;
}

bool check_position_clear(grid_t *grid, int32_t *p)
{
    return grid->array[p[0]][p[1]] != grid->fill_state;
}

void set_particle(grid_t *grid, int32_t *pos)
{
    grid->array[pos[0]][pos[1]] = grid->fill_state;
    grid->current_particles++;

    if (pos[0] > 0 && grid->array[pos[0] - 1][pos[1]] != grid->fill_state)
    {
        grid->array[pos[0] - 1][pos[1]] = grid->adjacent_state;
    }
    if (pos[0] < grid->size - 1 && grid->array[pos[0] + 1][pos[1]] != grid->fill_state)
    {
        grid->array[pos[0] + 1][pos[1]] = grid->adjacent_state;
    }
    if (pos[1] > 0 && grid->array[pos[0]][pos[1] - 1] != grid->fill_state)
    {
        grid->array[pos[0]][pos[1] - 1] = grid->adjacent_state;
    }
    if (pos[1] < grid->size - 1 && grid->array[pos[0]][pos[1] + 1] != grid->fill_state)
    {
        grid->array[pos[0]][pos[1] + 1] = grid->adjacent_state;
    }
}

void grid_run_simulation(grid_t *grid)
{
    printf("[DEBUG] Starting simulation\n");

    // Set the origin particle
    uint32_t pos[2] = {rand() % grid->size, rand() % grid->size};
    while (!check_particle_inside(grid, pos))
    {
        pos[0] = rand() % grid->size;
        pos[1] = rand() % grid->size;
    }

    set_particle(grid, pos);

    printf("[DEBUG] Simulation began\n");

    while (grid->current_particles < grid->max_particles)
    {
        uint32_t particle_iterations = 0;
        bool is_ready = false;

        // Initialize a new particle position until valid
        while (!is_ready)
        {
            pos[0] = rand() % grid->size;
            pos[1] = rand() % grid->size;

            if (check_particle_inside(grid, pos) && check_position_clear(grid, pos))
            {
                is_ready = true;
            }
        }

        printf("[DEBUG] Particle %d ready at %d, %d\n", grid->current_particles, pos[0], pos[1]);

        // Iteration loop for moving the particle
        while (particle_iterations < grid->iteration_max)
        {
            printf("[DEBUG] Particle %d beginning iteration %d\n", grid->current_particles, particle_iterations);

            int32_t new_pos[2] = {pos[0], pos[1]};
            uint8_t move_dir = rand() % 4;
            bool valid_move = false;

            // Try to move in a random direction
            switch (move_dir)
            {
            case 0: // Left
                if (check_particle_inside(grid, (int32_t[]){pos[0] - 1, pos[1]}) && check_position_clear(grid, (int32_t[]){pos[0] - 1, pos[1]}))
                {
                    new_pos[0]--;
                    valid_move = true;
                }
                break;
            case 1: // Right
                if (check_particle_inside(grid, (int32_t[]){pos[0] + 1, pos[1]}) && check_position_clear(grid, (int32_t[]){pos[0] + 1, pos[1]}))
                {
                    new_pos[0]++;
                    valid_move = true;
                }
                break;
            case 2: // Up
                if (check_particle_inside(grid, (int32_t[]){pos[0], pos[1] - 1}) && check_position_clear(grid, (int32_t[]){pos[0], pos[1] - 1}))
                {
                    new_pos[1]--;
                    valid_move = true;
                }
                break;
            case 3: // Down
                if (check_particle_inside(grid, (int32_t[]){pos[0], pos[1] + 1}) && check_position_clear(grid, (int32_t[]){pos[0], pos[1] + 1}))
                {
                    new_pos[1]++;
                    valid_move = true;
                }
                break;
            }

            // If valid move, update particle position
            if (valid_move)
            {
                pos[0] = new_pos[0];
                pos[1] = new_pos[1];

                // Check if the particle is adjacent before setting it
                if (check_particle_adjecent(grid, pos))
                {
                    set_particle(grid, pos);
                    printf("[DEBUG] Particle placed at %d, %d\n", pos[0], pos[1]);
                    break; // Exit the iteration loop as the particle is placed
                }
            }

            particle_iterations++;
        }

        // If max iterations are reached and the particle is adjacent, place the particle
        if (particle_iterations >= grid->iteration_max)
        {
            // Ensure that the particle is only placed if it's adjacent to another particle
            if (check_particle_adjecent(grid, pos))
            {
                set_particle(grid, pos);
                printf("[DEBUG] Particle placed at %d, %d after max iterations\n", pos[0], pos[1]);
            }
            else
            {
                printf("[DEBUG] Particle discarded after max iterations at %d, %d - not adjacent\n", pos[0], pos[1]);
            }
        }
    }
}

void grid_free(grid_t *grid)
{
    for (int i = 0; i < grid->size; i++)
    {
        free(grid->array[i]); // free the rows
    }
    free(grid->array); // free columns

    free(grid);
}
