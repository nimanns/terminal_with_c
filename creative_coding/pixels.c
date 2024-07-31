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

void draw_fractal_tree(HDC hdc, int x1, int y1, float angle, int depth) {
    if (depth == 0) return;

    int x2 = x1 + (int)(cos(angle) * depth * 10.0);
    int y2 = y1 + (int)(sin(angle) * depth * 10.0);

    draw_line(hdc, x1, y1, x2, y2, RGB(0, 255 / (depth + 1), 0));

    draw_fractal_tree(hdc, x2, y2, angle - 0.5, depth - 1);
    draw_fractal_tree(hdc, x2, y2, angle + 0.5, depth - 1);
}

int main() {
    int width = 800;
    int height = 600;
    HDC hdc = GetDC(NULL);

    draw_fractal_tree(hdc, width / 2, height, -3.14 / 2, 10);

    ReleaseDC(NULL, hdc);
    getchar();
    return 0;
}
