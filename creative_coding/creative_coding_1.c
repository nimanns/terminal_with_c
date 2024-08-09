#include <windows.h>
#include <math.h>

LRESULT CALLBACK window_procedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
void draw_fractal(HDC hdc, int x, int y, int size, int depth);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev_instance, LPSTR lpcmd_line, int ncmd_show) {
    // ... (same as previous)
}

LRESULT CALLBACK window_procedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    // ... (same as previous)
}

void draw_fractal(HDC hdc, int x, int y, int size, int depth) {
    if (depth == 0) return;

    MoveToEx(hdc, x, y, NULL);
    LineTo(hdc, x, y - size);

    int new_x = x;
    int new_y = y - size;

    draw_fractal(hdc, new_x, new_y, size * 0.8, depth - 1);
    draw_fractal(hdc, new_x - sin(3.14/4) * size, new_y - cos(3.14/4) * size, size * 0.7, depth - 1);
    draw_fractal(hdc, new_x + sin(3.14/4) * size, new_y - cos(3.14/4) * size, size * 0.7, depth - 1);
}
