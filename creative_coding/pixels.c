#include <windows.h>
#include <stdio.h>
#include <math.h>

int main() {
    int width = 800;
    int height = 600;
    HDC hdc = GetDC(NULL);
    int x, y;
    float time = 0;

    for (int i = 0; i < 1000; i++) {
        time += 0.1;
        x = width / 2 + sin(time) * width / 4;
        y = height / 2 + cos(time) * height / 4;
        SetPixel(hdc, x, y, RGB(rand() % 256, rand() % 256, rand() % 256));
        Sleep(10);
    }

    ReleaseDC(NULL, hdc);
    return 0;
}
