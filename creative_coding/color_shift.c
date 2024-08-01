#include <windows.h>
#include <stdlib.h>
#include <time.h>

#define SHIFT_SPEED 1

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int blueShift = 0;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "BlueShift";
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    
    RegisterClass(&wc);
    HWND hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_LAYERED,
        CLASS_NAME,
        "Blue Shift Effect",
        WS_POPUP,
        0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
        NULL, NULL, hInstance, NULL
    );

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            SetTimer(hwnd, 1, 50, NULL);  // 50ms timer for smooth animation
        } break;

        case WM_TIMER: {
            blueShift += SHIFT_SPEED;
            if (blueShift > 255) blueShift = 255;
            
            BYTE alpha = 255 - blueShift;  // Fade out as blue increases
            SetLayeredWindowAttributes(hwnd, 0, alpha, LWA_ALPHA);

            InvalidateRect(hwnd, NULL, FALSE);
        } break;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rect;
            GetClientRect(hwnd, &rect);
            
            // Create a blue brush with increasing intensity
            HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, blueShift));
            FillRect(hdc, &rect, hBrush);
            DeleteObject(hBrush);

            EndPaint(hwnd, &ps);
        } break;

        case WM_DESTROY:
            KillTimer(hwnd, 1);
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

