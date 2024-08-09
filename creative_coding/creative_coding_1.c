#include <windows.h>
#include <math.h>

LRESULT CALLBACK window_procedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
void draw_fractal(HDC hdc, int x, int y, int size, int depth);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev_instance, LPSTR lpcmd_line, int ncmd_show) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = window_procedure;
    wc.hInstance = hinstance;
    wc.lpszClassName = "fractal_window";
    RegisterClass(&wc);

    HWND hwnd = CreateWindow("fractal_window", "Fractal Tree", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 800, 600, NULL, NULL, hinstance, NULL);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK window_procedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            draw_fractal(hdc, 400, 550, 100, 5);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
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
