#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define NUM_STARS 100
#define PI 3.14159265358979323846

typedef struct {
    double x;
    double y;
    double speed;
    double angle;
    int size;
    COLORREF color;
} Star;

Star stars[NUM_STARS];

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "FloatingStars";
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    
    RegisterClass(&wc);
    HWND hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT,
        CLASS_NAME,
        "Floating Stars",
        WS_POPUP,
        0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
        NULL, NULL, hInstance, NULL
    );
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
    ShowWindow(hwnd, nCmdShow);

    srand(time(NULL));
    for (int i = 0; i < NUM_STARS; i++) {
        stars[i].x = rand() % GetSystemMetrics(SM_CXSCREEN);
        stars[i].y = rand() % GetSystemMetrics(SM_CYSCREEN);
        stars[i].speed = (rand() % 100) / 100.0 + 0.5; // Speed between 0.5 and 1.5
        stars[i].angle = (rand() % 360) * PI / 180.0;
        stars[i].size = rand() % 3 + 1; // Size between 1 and 3
        stars[i].color = RGB(rand() % 156 + 100, rand() % 156 + 100, rand() % 156 + 100);
    }

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

void DrawStar(HDC hdc, int x, int y, int size, COLORREF color) {
    for (int dy = -size; dy <= size; dy++) {
        for (int dx = -size; dx <= size; dx++) {
            if (dx*dx + dy*dy <= size*size) {
                SetPixel(hdc, x + dx, y + dy, color);
            }
        }
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rect;
            GetClientRect(hwnd, &rect);
            FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

            for (int i = 0; i < NUM_STARS; i++) {
                DrawStar(hdc, (int)stars[i].x, (int)stars[i].y, stars[i].size, stars[i].color);

                stars[i].x += cos(stars[i].angle) * stars[i].speed;
                stars[i].y += sin(stars[i].angle) * stars[i].speed;

                // Wrap around screen edges
                if (stars[i].x < 0) stars[i].x = rect.right;
                if (stars[i].x > rect.right) stars[i].x = 0;
                if (stars[i].y < 0) stars[i].y = rect.bottom;
                if (stars[i].y > rect.bottom) stars[i].y = 0;

                // Occasionally change direction
                if (rand() % 100 == 0) {
                    stars[i].angle = (rand() % 360) * PI / 180.0;
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
