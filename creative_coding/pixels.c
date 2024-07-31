#include <windows.h>
#include <stdio.h>
#include <math.h>

void draw_line(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        SetPixel(hdc, x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx) { err += dx; y1 += sy; }
    }
}

int main() {
    int width = 800;
    int height = 600;
    HDC hdc = GetDC(NULL);
    int x1, y1, x2, y2;

    for (int i = 0; i < 100; i++) {
        x1 = rand() % width;
        y1 = rand() % height;
        x2 = rand() % width;
        y2 = rand() % height;
        draw_line(hdc, x1, y1, x2, y2, RGB(rand() % 256, rand() % 256, rand() % 256));
        Sleep(50);
    }

    ReleaseDC(NULL, hdc);
    return 0;
}
