#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

#define WIDTH 80
#define HEIGHT 40
#define NUM_CELL_TYPES 5

typedef struct {
    char symbol;
    int color;
} CellType;

CellType cellTypes[NUM_CELL_TYPES] = {
    {' ', 0},      // Empty
    {'■', 4},      // Red
    {'■', 2},      // Green
    {'■', 1},      // Blue
    {'■', 6}       // Yellow
};

int grid[HEIGHT][WIDTH];
int newGrid[HEIGHT][WIDTH];

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void initializeGrid() {
    srand(time(NULL));
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            grid[y][x] = rand() % NUM_CELL_TYPES;
        }
    }
}

int countNeighbors(int x, int y, int type) {
    int count = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;
            int nx = (x + i + WIDTH) % WIDTH;
            int ny = (y + j + HEIGHT) % HEIGHT;
            if (grid[ny][nx] == type) count++;
        }
    }
    return count;
}

void updateGrid() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int currentType = grid[y][x];
            int neighbors[NUM_CELL_TYPES] = {0};
            
            for (int t = 1; t < NUM_CELL_TYPES; t++) {
                neighbors[t] = countNeighbors(x, y, t);
            }
            
            if (currentType == 0) {
                // Empty cell
                int maxNeighbors = 0;
                int dominantType = 0;
                for (int t = 1; t < NUM_CELL_TYPES; t++) {
                    if (neighbors[t] > maxNeighbors) {
                        maxNeighbors = neighbors[t];
                        dominantType = t;
                    }
                }
                if (maxNeighbors == 3) {
                    newGrid[y][x] = dominantType;
                } else {
                    newGrid[y][x] = 0;
                }
            } else {
                // Non-empty cell
                int totalNeighbors = 0;
                for (int t = 1; t < NUM_CELL_TYPES; t++) {
                    totalNeighbors += neighbors[t];
                }
                if (totalNeighbors < 2 || totalNeighbors > 3) {
                    newGrid[y][x] = 0;
                } else {
                    newGrid[y][x] = currentType;
                }
            }
        }
    }
    
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            grid[y][x] = newGrid[y][x];
        }
    }
}

void displayGrid() {
    COORD coord = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int cellType = grid[y][x];
            setColor(cellTypes[cellType].color);
            putchar(cellTypes[cellType].symbol);
        }
        putchar('\n');
    }
}

int main() {
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 100;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);

    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(consoleHandle, &csbi);
    COORD bufferSize = {WIDTH, HEIGHT};
    SetConsoleScreenBufferSize(consoleHandle, bufferSize);

    SMALL_RECT windowSize = {0, 0, WIDTH - 1, HEIGHT - 1};
    SetConsoleWindowInfo(consoleHandle, TRUE, &windowSize);

    initializeGrid();
    
    while (1) {
        displayGrid();
        updateGrid();
        Sleep(50);  // Pause for 50 milliseconds
    }

    return 0;
}
