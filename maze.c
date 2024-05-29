#include <stdio.h>
#include <stdlib.h>
#include "maze.h"

/**
 * @brief Initialise a maze object - allocate memory and set attributes
 *
 * @param this pointer to the maze to be initialised
 * @param height height to allocate
 * @param width width to allocate
 * @return int 0 on success, 1 on fail
 */
int create_maze(maze *this, int height, int width) {
    this->height = height;
    this->width = width;
    this->map = malloc(height * sizeof(char *));
    if (!this->map) return 1;
    for (int i = 0; i < height; i++) {
        this->map[i] = malloc((width + 1) * sizeof(char)); // +1 for null terminator
        if (!this->map[i]) return 1;
    }
    return 0;
}

/**
 * @brief Free the memory allocated to the maze struct
 *
 * @param this the pointer to the struct to free
 */
void free_maze(maze *this) {
    for (int i = 0; i < this->height; i++) {
        free(this->map[i]);
    }
    free(this->map);
}

/**
 * @brief Validate and return the width of the mazefile
 *
 * @param file the file pointer to check
 * @return int 0 for error, or a valid width (5-100)
 */
int get_width(FILE *file) {
    int width = 0;
    char ch;
    while ((ch = fgetc(file)) != EOF && ch != '\n') {
        width++;
    }
    if (width < MIN_DIM || width > MAX_DIM) {
        return 0;
    }
    return width;
}

/**
 * @brief Validate and return the height of the mazefile
 *
 * @param file the file pointer to check
 * @return int 0 for error, or a valid height (5-100)
 */
int get_height(FILE *file) {
    int height = 0;
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            height++;
        }
    }
    if (height < MIN_DIM || height > MAX_DIM) {
        return 0;
    }
    return height;
}

// if char is useful
int is_valid_char(char ch) {
    return ch == 'S' || ch == 'E' || ch == '#' || ch == ' ';
}

//check if it is cross
int is_reachable(maze *this, int x, int y, int **visited) {
    if (x < 0 || x >= this->width || y < 0 || y >= this->height || this->map[y][x] == '#' || visited[y][x]) {
        return 0;
    }

    if (x == this->end.x && y == this->end.y) {
        return 1;
    }

    visited[y][x] = 1;

    if (is_reachable(this, x + 1, y, visited) ||
        is_reachable(this, x - 1, y, visited) ||
        is_reachable(this, x, y + 1, visited) ||
        is_reachable(this, x, y - 1, visited)) {
        return 1;
    }

    return 0;
}

//check vaildate and only s and e
int validate_maze(maze *this) {
    int found_start = 0, found_end = 0;
    for (int i = 0; i < this->height; i++) {
        for (int j = 0; j < this->width; j++) {
            if (!is_valid_char(this->map[i][j])) {
                return 1; // illegal char
            }
            if (this->map[i][j] == 'S') {
                if (found_start) return 1; // multiple s
                this->start.x = j;
                this->start.y = i;
                found_start = 1;
            }
            if (this->map[i][j] == 'E') {
                if (found_end) return 1; // multiple n
                this->end.x = j;
                this->end.y = i;
                found_end = 1;
            }
        }
    }

    if (!found_start || !found_end) {
        return 1; // no s or n
    }

    // if it cross
    int **visited = malloc(this->height * sizeof(int *));
    for (int i = 0; i < this->height; i++) {
        visited[i] = calloc(this->width, sizeof(int));
    }

    int result = is_reachable(this, this->start.x, this->start.y, visited);

    for (int i = 0; i < this->height; i++) {
        free(visited[i]);
    }
    free(visited);

    return result ? 0 : 1;
}

/**
 * @brief read in a maze file into a struct
 *
 * @param this Maze struct to be used
 * @param file Maze file pointer
 * @return int 0 on success, 1 on fail
 */
int read_maze(maze *this, FILE *file) {
    for (int i = 0; i < this->height; i++) {
        if (fgets(this->map[i], this->width + 2, file) == NULL) { // +2 以考虑换行符和空终止符
            return 1;
        }
    }

    return validate_maze(this);
}

/**
 * @brief Prints the maze out - code provided to ensure correct formatting
 *
 * @param this pointer to maze to print
 * @param player the current player location
 */
void print_maze(maze *this, coord *player) {
    printf("\n");
    for (int i = 0; i < this->height; i++) {
        for (int j = 0; j < this->width; j++) {
            if (player->x == j && player->y == i) {
                printf("X");
            } else {
                printf("%c", this->map[i][j]);
            }
        }
        printf("\n");
    }
}

/**
 * @brief Validates and performs a movement in a given direction
 *
 * @param this Maze struct
 * @param player The player's current position
 * @param direction The desired direction to move in
 */
void move(maze *this, coord *player, char direction) {
    int new_x = player->x;
    int new_y = player->y;

    if (direction == 'w' || direction == 'W') new_y--;
    else if (direction == 's' || direction == 'S') new_y++;
    else if (direction == 'a' || direction == 'A') new_x--;
    else if (direction == 'd' || direction == 'D') new_x++;

    if (new_x < 0 || new_x >= this->width || new_y < 0 || new_y >= this->height || this->map[new_y][new_x] == '#') {
        printf("Invalid move.\n");
    } else {
        player->x = new_x;
        player->y = new_y;
    }
}

/**
 * @brief Check whether the player has won and return a pseudo-boolean
 *
 * @param this current maze
 * @param player player position
 * @return int 0 for false, 1 for true
 */
int has_won(maze *this, coord *player) {
    return player->x == this->end.x && player->y == this->end.y;
}
