#include <windows.h>
#include <stdlib.h>
#include <time.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "Random Shapes Window Class";
    
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    
    RegisterClass(&wc);
    
    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Random Shapes",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL,
        NULL,
        hInstance,
        NULL
    );
    
    if (hwnd == NULL) {
        return 0;
    }
    
    ShowWindow(hwnd, nCmdShow);
    
    srand(time(NULL));
    
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            int width = clientRect.right - clientRect.left;
            int height = clientRect.bottom - clientRect.top;
            
            for (int i = 0; i < 50; i++) {
                int shape = rand() % 3;
                int x = rand() % width;
                int y = rand() % height;
                int size = rand() % 100 + 20;
                HBRUSH brush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
                SelectObject(hdc, brush);
                
                switch (shape) {
                    case 0: // Circle
                        Ellipse(hdc, x, y, x + size, y + size);
                        break;
                    case 1: // Rectangle
                        Rectangle(hdc, x, y, x + size, y + size);
                        break;
                    case 2: // Triangle
                        {
                            POINT points[3] = {
                                {x, y + size},
                                {x + size / 2, y},
                                {x + size, y + size}
                            };
                            Polygon(hdc, points, 3);
                        }
                        break;
                }
                
                DeleteObject(brush);
            }
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
        case WM_LBUTTONDOWN: {
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
