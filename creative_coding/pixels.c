#include <windows.h>
#include <stdio.h>

int main() {
    int width = 800;
    int height = 600;
    HDC hdc = GetDC(NULL);
    int x, y;

    for (int i = 0; i < 1000; i++) {
        x = rand() % width;
        y = rand() % height;
        SetPixel(hdc, x, y, RGB(rand() % 256, rand() % 256, rand() % 256));
        Sleep(10);
    }

    ReleaseDC(NULL, hdc);
    return 0;
}
