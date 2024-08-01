#include <windows.h>
#include <stdlib.h>
#include <time.h>

#define NUM_DROPS 100
#define MAX_LENGTH 20

typedef struct {
    int x;
    int y;
    int length;
    COLORREF color;
} Raindrop;

Raindrop raindrops[NUM_DROPS];

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "RainEffect";

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT,
        CLASS_NAME,
        "Rain Effect",
        WS_POPUP,
        0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
        NULL, NULL, hInstance, NULL
    );

    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

    ShowWindow(hwnd, nCmdShow);

    srand(time(NULL));

    for (int i = 0; i < NUM_DROPS; i++) {
        raindrops[i].x = rand() % GetSystemMetrics(SM_CXSCREEN);
        raindrops[i].y = rand() % GetSystemMetrics(SM_CYSCREEN);
        raindrops[i].length = rand() % MAX_LENGTH + 1;
        raindrops[i].color = RGB(rand() % 256, rand() % 256, rand() % 256);
    }

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rect;
            GetClientRect(hwnd, &rect);
            FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

            for (int i = 0; i < NUM_DROPS; i++) {
                for (int j = 0; j < raindrops[i].length; j++) {
                    SetPixel(hdc, raindrops[i].x, raindrops[i].y + j, raindrops[i].color);
                }

                raindrops[i].y += 2;
                if (raindrops[i].y > rect.bottom) {
                    raindrops[i].x = rand() % rect.right;
                    raindrops[i].y = 0;
                    raindrops[i].length = rand() % MAX_LENGTH + 1;
                    raindrops[i].color = RGB(rand() % 256, rand() % 256, rand() % 256);
                }
            }

            EndPaint(hwnd, &ps);
            InvalidateRect(hwnd, NULL, FALSE);
        } break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
