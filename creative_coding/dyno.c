#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>

#define GROUND_Y 20
#define DINO_X 10
#define OBSTACLE_START_X 80

int getChar();
void clearScreen();
void drawScene(int dinoY, int obstacleX);

int main() {
    int dinoY = GROUND_Y;
    int obstacleX = OBSTACLE_START_X;
    int score = 0;
    int jumping = 0;
    int jumpHeight = 0;

    struct termios old_tio, new_tio;
    tcgetattr(STDIN_FILENO, &old_tio);
    new_tio = old_tio;
    new_tio.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);

    while (1) {
        if (getChar() == ' ' && !jumping) {
            jumping = 1;
        }

        if (jumping) {
            if (jumpHeight < 10) {
                dinoY--;
                jumpHeight++;
            } else {
                dinoY++;
                jumpHeight--;
                if (jumpHeight == 0) {
                    jumping = 0;
                }
            }
        }

        obstacleX--;
        if (obstacleX <= 0) {
            obstacleX = OBSTACLE_START_X;
            score++;
        }

        if (obstacleX == DINO_X && dinoY == GROUND_Y) {
            break;
        }

        clearScreen();
        drawScene(dinoY, obstacleX);
        printf("Score: %d\n", score);

        usleep(50000);
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
    printf("\nGame Over! Final Score: %d\n", score);
    return 0;
}

int getChar() {
    int ch = getchar();
    return (ch == EOF) ? 0 : ch;
}

void clearScreen() {
    printf("\033[H\033[J");
}

void drawScene(int dinoY, int obstacleX) {
    for (int y = 0; y < GROUND_Y; y++) {
        for (int x = 0; x < OBSTACLE_START_X; x++) {
            if (x == DINO_X && y == dinoY) {
                printf("D");
            } else if (x == obstacleX && y == GROUND_Y) {
                printf("X");
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
    for (int x = 0; x < OBSTACLE_START_X; x++) {
        printf("=");
    }
    printf("\n");
}
