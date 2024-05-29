#include <stdio.h>
#include <stdlib.h>
#include "maze.h"

// Main function: Entry point of the program
int main(int argc, char *argv[]) {
    // Check if the number of command-line arguments is correct
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <mazefile path>\n", argv[0]);
        return EXIT_ARG_ERROR;
    }

    maze this_maze;
    FILE *f = fopen(argv[1], "r"); // Open the specified maze file
    if (!f) {
        perror("Error opening file");
        return EXIT_FILE_ERROR;
    }

    int width = get_width(f);
    fseek(f, 0, SEEK_SET);// Reset the file pointer
    int height = get_height(f);
    if (width == 0 || height == 0) {
        fclose(f);
        fprintf(stderr, "Invalid maze dimensions.\n");
        return EXIT_MAZE_ERROR;
    }

    if (create_maze(&this_maze, height, width)) {
        fclose(f);
        fprintf(stderr, "Memory allocation error.\n");
        return EXIT_MAZE_ERROR;
    }

    fseek(f, 0, SEEK_SET);
    // Read the maze data into the structure
    if (read_maze(&this_maze, f)) {
        fclose(f);
        free_maze(&this_maze);
        fprintf(stderr, "Error reading maze.\n");
        return EXIT_MAZE_ERROR;
    }
    fclose(f);

    coord player = this_maze.start;// Set the player's initial position to the maze start

    char command;// To store the user's command input
    while (1) {
        printf("Enter command (WASD to move, M to show map, Q to quit): ");
        scanf(" %c", &command);

        if (command == 'q' || command == 'Q') break;
        if (command == 'm' || command == 'M') {
            print_maze(&this_maze, &player);
        } else {
            move(&this_maze, &player, command);
            if (has_won(&this_maze, &player)) {
                printf("Congratulations! You reached the exit!\n");
                break;
            }
        }
    }

    free_maze(&this_maze);// Free the allocated memory
    return EXIT_SUCCESS;
}
