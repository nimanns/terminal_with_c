#include <stdio.h>
#include <unistd.h>

void clear() {
    printf("\033[H\033[J");
}

void draw_square(int size, int x, int y, char *color) {
    printf("\033[%sm", color);
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            printf("\033[%d;%dH*", y + i, x + j);
        }
    }
    printf("\033[0m");
}

int main() {
    int x = 1, y = 1, size = 5;
    char *colors[] = {"31", "32", "33", "34", "35", "36"};
    int color_index = 0;

    while (1) {
        clear();
        draw_square(size, x, y, colors[color_index]);
        fflush(stdout);
        usleep(200000);

        x += 2; y += 1; color_index = (color_index + 1) % 6;
        if (x > 40) x = 1;
        if (y > 20) y = 1;
    }

    return 0;
}
