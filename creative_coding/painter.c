#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define screen_width 80
#define screen_height 24

void clear_screen() {
    printf("\033[2J\033[H");
}

void draw_screen(char screen[screen_height][screen_width]) {
    for (int i = 0; i < screen_height; i++) {
        for (int j = 0; j < screen_width; j++) {
            putchar(screen[i][j]);
        }
        putchar('\n');
    }
}

int main() {
    char screen[screen_height][screen_width];
    int x = screen_width / 2;
    int y = screen_height / 2;
    int dx = 1;
    int dy = 1;
    char trail_char = '*';

    srand(time(NULL));

    while (1) {
        memset(screen, ' ', sizeof(screen));

        x += dx;
        y += dy;

        if (x <= 0 || x >= screen_width - 1) {
            dx = -dx;
            trail_char = rand() % 26 + 'A';
        }
        if (y <= 0 || y >= screen_height - 1) {
            dy = -dy;
            trail_char = rand() % 26 + 'a';
        }

        screen[y][x] = 'O';

        for (int i = 1; i <= 5; i++) {
            int trail_x = x - i * dx;
            int trail_y = y - i * dy;
            if (trail_x >= 0 && trail_x < screen_width && trail_y >= 0 && trail_y < screen_height) {
                screen[trail_y][trail_x] = trail_char;
            }
        }

        clear_screen();
        draw_screen(screen);
        usleep(100000);
    }

    return 0;
}
