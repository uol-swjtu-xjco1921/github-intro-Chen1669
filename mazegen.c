#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<time.h>

//define initial maze

//define max and min dimensions
#define MAX_DIM 100
#define MIN_DIM 5
//requier code
#define EXIT_SUCCESS 0
#define EXIT_ARG_ERROR 1
#define EXIT_FILE_ERROR 2
#define EXIT_MAZE_ERROR 3

//define maze struct
typedef struct __Coord
{
    int x;
    int y;
} coord;

typedef struct __Maze
{
    char **map;
    int height;
    int width;
    coord start;
    coord end;
} maze;

//creat initial maze
int create_maze(maze *this, int height, int width)
{
    this->height = height;
    this->width = width;
    this->map = malloc(height * sizeof(char *));
    for (int i = 0; i < height; i++) {
        this->map[i] = malloc((width + 1) * sizeof(char)); 
        for(int j=0;j<width;j++){
            this->map[i][j]='#';
        }
    }
    return 0;
}

//free memroy
void free_maze(maze *this)
{
    for (int i = 0; i < this->height; i++) {
        free(this->map[i]);
    }
    free(this->map);
}

//use DFS to creat maze

//random direction
void shuffle(coord *array, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        coord temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}
//creat passage
void passage(int cx, int cy, maze *this) {
    coord directions[] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    shuffle(directions, 4);//move randomly
    
    for (int i = 0; i < 4; i++) {
        int nx = cx + directions[i].x * 2;
        int ny = cy + directions[i].y * 2;//move 2

        if (nx >= 0 && ny >= 0 && nx < this->width && ny < this->height && this->map[ny][nx] == '#') {
            this->map[ny][nx] = ' ';
            this->map[cy + directions[i].y][cx + directions[i].x] = ' ';
            passage(nx, ny, this);//make sure moving is useful and change'#' to ' '
        }
    }
}

//save the maze
void generate_maze(maze *m) {
    create_maze(m, m->width, m->height);
    m->start.x = 1;
    m->start.y = 1;
    m->end.x = m->width - 2;
    m->end.y = m->height - 2;

    m->map[m->start.y][m->start.x] = 'S';
    m->map[m->end.y][m->end.x] = 'E';//provide initial data

    passage(m->start.x, m->start.y, m);
}

void save_maze(maze *m, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < m->height; i++) {
        for (int j = 0; j < m->width; j++) {
            fputc(m->map[i][j], file);
        }
        fputc('\n', file);
    }

    fclose(file);
}

//main fuction
int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <filename> <width> <height>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *filename = argv[1];
    int width = atoi(argv[2]);
    int height = atoi(argv[3]);

    if (width < MIN_DIM || width > MAX_DIM || height < MIN_DIM || height > MAX_DIM) {
        fprintf(stderr, "Invalid dimensions. Width and height must be between %d and %d.\n", MIN_DIM, MAX_DIM);
        return EXIT_FAILURE;
    }

    srand(time(NULL));

    maze m;
    m.width = width;
    m.height = height;
    generate_maze(&m);
    save_maze(&m, filename);
    free_maze(&m);

    printf("Maze saved to %s\n", filename);

    return EXIT_SUCCESS;
}

