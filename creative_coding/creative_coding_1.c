#include <windows.h>

LRESULT CALLBACK window_procedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
void draw_fractal(HDC hdc, int x, int y, int size);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev_instance, LPSTR lpcmd_line, int ncmd_show) {
    // ... (same as previous)
}

LRESULT CALLBACK window_procedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            draw_fractal(hdc, 400, 550, 100);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

void draw_fractal(HDC hdc, int x, int y, int size) {
    MoveToEx(hdc, x, y, NULL);
    LineTo(hdc, x, y - size);
}
