#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#define WIDTH 80
#define HEIGHT 25
#define RAIN_CHANCE 10
#define RAIN_CHAR '|'
#define SPLASH_CHAR '-'

char screen[HEIGHT][WIDTH];
int splash[WIDTH];

void initializeScreen() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            screen[y][x] = ' ';
        }
    }
    memset(splash, 0, sizeof(splash));
}

void updateRain() {
    // Move existing rain down
    for (int y = HEIGHT - 1; y > 0; y--) {
        for (int x = 0; x < WIDTH; x++) {
            screen[y][x] = screen[y-1][x];
        }
    }

    // Generate new rain
    for (int x = 0; x < WIDTH; x++) {
        if (rand() % 100 < RAIN_CHANCE) {
            screen[0][x] = RAIN_CHAR;
        } else {
            screen[0][x] = ' ';
        }
    }

    // Update splash
    for (int x = 0; x < WIDTH; x++) {
        if (screen[HEIGHT-1][x] == RAIN_CHAR) {
            splash[x] = 2;
        } else if (splash[x] > 0) {
            splash[x]--;
        }
    }
}

void drawScreen() {
    // Move cursor to top-left corner
    printf("\033[H");

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (y == HEIGHT - 1 && splash[x] > 0) {
                printf("\033[36m%c\033[0m", SPLASH_CHAR);  // Cyan color for splash
            } else if (screen[y][x] == RAIN_CHAR) {
                printf("\033[94m%c\033[0m", screen[y][x]);  // Light blue for rain
            } else {
                printf("%c", screen[y][x]);
            }
        }
        printf("\n");
    }
}

int main() {
    // Enable ANSI escape sequences
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);

    // Hide cursor
    printf("\033[?25l");

    // Set console size
    char cmd[100];
    snprintf(cmd, sizeof(cmd), "mode con: cols=%d lines=%d", WIDTH, HEIGHT);
    system(cmd);

    srand(time(NULL));
    initializeScreen();

    while (1) {
        updateRain();
        drawScreen();
        Sleep(100);  // Wait for 100 milliseconds
    }

    // Show cursor (this part is never reached in this example)
    printf("\033[?25h");

    return 0;
}
