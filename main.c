#include <stdio.h>
#include <stdlib.h>
#include "maze.h"

int main(int argc, char *argv[]) {
    // Check if the correct number of command-line arguments is provided
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <mazefile path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Declare variables
    maze this_maze;
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // Read the dimensions of the maze from the file
    int width = get_width(f);
    fseek(f, 0, SEEK_SET);
    int height = get_height(f);
    if (width == 0 || height == 0) {
        fclose(f);
        fprintf(stderr, "Invalid maze dimensions.\n");
        return EXIT_FAILURE;
    }

    // Create a maze structure
    if (create_maze(&this_maze, height, width)) {
        fclose(f);
        fprintf(stderr, "Memory allocation error.\n");
        return EXIT_FAILURE;
    }

    // Read the maze data into the structure
    fseek(f, 0, SEEK_SET);
    if (read_maze(&this_maze, f)) {
        fclose(f);
        free_maze(&this_maze);
        fprintf(stderr, "Error reading maze.\n");
        return EXIT_FAILURE;
    }
    fclose(f);

    // Set the player's initial position to the maze start
    coord player = this_maze.start;

    // Prompt the user for the first command
    char command;
    printf("Enter command (WASD to move, M to show map, Q to quit): ");
    while (1) {
        // Read the command from the user
        scanf(" %c", &command);

        // Check the command
        if (command == 'q' || command == 'Q') break;
        if (command == 'm' || command == 'M') {
            // Show the map
            print_maze(&this_maze, &player);
        } else {
            // Move the player
            move(&this_maze, &player, command);
            if (has_won(&this_maze, &player)) {
                // Check if the player has won
                printf("Congratulations! You reached the exit!\n");
                break;
            }
        }
    }

    // Free memory allocated for the maze
    free_maze(&this_maze);
    return EXIT_SUCCESS;
}
