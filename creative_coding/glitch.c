#include <windows.h>
#include <stdlib.h>
#include <time.h>

#define GLITCH_DURATION 5000
#define GLITCH_INTERVAL 50

void glitch_effect(HDC hdc, int width, int height) {
    int x, y, w, h;
    HBITMAP hbm_screen;
    HDC hdc_mem;

    hdc_mem = CreateCompatibleDC(hdc);
    hbm_screen = CreateCompatibleBitmap(hdc, width, height);
    SelectObject(hdc_mem, hbm_screen);

    BitBlt(hdc_mem, 0, 0, width, height, hdc, 0, 0, SRCCOPY);

    for (int i = 0; i < 50; i++) {
        x = rand() % width;
        y = rand() % height;
        w = rand() % (width - x);
        h = rand() % (height - y);

        int dx = (rand() % 21) - 10;
        int dy = (rand() % 21) - 10;

        BitBlt(hdc, x + dx, y + dy, w, h, hdc_mem, x, y, SRCCOPY);
    }

    DeleteObject(hbm_screen);
    DeleteDC(hdc_mem);
}

int main() {
    HDC hdc_screen = GetDC(NULL);
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    srand(time(NULL));

    DWORD start_time = GetTickCount();
    while (GetTickCount() - start_time < GLITCH_DURATION) {
        glitch_effect(hdc_screen, width, height);
        Sleep(GLITCH_INTERVAL);
    }

    ReleaseDC(NULL, hdc_screen);
    return 0;
}
