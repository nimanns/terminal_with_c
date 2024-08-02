#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MAX_BALLS 100
#define TIMER_ID 1
#define TIMER_INTERVAL 30

typedef struct {
    float x, y;
    float vx, vy;
    float radius;
    COLORREF color;
} Ball;

Ball balls[MAX_BALLS];
int ballCount = 0;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CreateBall(int x, int y);
void UpdateBalls(HWND hwnd);
void DrawBalls(HDC hdc, RECT* clientRect);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "Bouncing Balls Window Class";
    
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    
    RegisterClass(&wc);
    
    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Bouncing Balls",
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
    SetTimer(hwnd, TIMER_ID, TIMER_INTERVAL, NULL);
    
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            KillTimer(hwnd, TIMER_ID);
            PostQuitMessage(0);
            return 0;
        
        case WM_LBUTTONDOWN: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            CreateBall(x, y);
            return 0;
        }
        
        case WM_TIMER:
            if (wParam == TIMER_ID) {
                UpdateBalls(hwnd);
                return 0;
            }
            break;
        
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            
            // Create a back buffer
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBitmap = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
            SelectObject(memDC, memBitmap);
            
            // Fill background
            HBRUSH bgBrush = CreateSolidBrush(RGB(240, 240, 240));
            FillRect(memDC, &clientRect, bgBrush);
            DeleteObject(bgBrush);
            
            DrawBalls(memDC, &clientRect);
            
            // Copy back buffer to screen
            BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, memDC, 0, 0, SRCCOPY);
            
            // Clean up
            DeleteObject(memBitmap);
            DeleteDC(memDC);
            
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void CreateBall(int x, int y) {
    if (ballCount < MAX_BALLS) {
        Ball* ball = &balls[ballCount++];
        ball->x = (float)x;
        ball->y = (float)y;
        ball->vx = (float)(rand() % 10 - 5);
        ball->vy = (float)(rand() % 10 - 5);
        ball->radius = (float)(rand() % 20 + 10);
        ball->color = RGB(rand() % 256, rand() % 256, rand() % 256);
    }
}

void UpdateBalls(HWND hwnd) {
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;
    
    for (int i = 0; i < ballCount; i++) {
        Ball* ball = &balls[i];
        
        ball->x += ball->vx;
        ball->y += ball->vy;
        
        if (ball->x - ball->radius < 0 || ball->x + ball->radius > width) {
            ball->vx = -ball->vx;
        }
        if (ball->y - ball->radius < 0 || ball->y + ball->radius > height) {
            ball->vy = -ball->vy;
        }
    }
    
    InvalidateRect(hwnd, NULL, FALSE);
}

void DrawBalls(HDC hdc, RECT* clientRect) {
    for (int i = 0; i < ballCount; i++) {
        Ball* ball = &balls[i];
        HBRUSH brush = CreateSolidBrush(ball->color);
        SelectObject(hdc, brush);
        Ellipse(hdc, (int)(ball->x - ball->radius), (int)(ball->y - ball->radius),
                (int)(ball->x + ball->radius), (int)(ball->y + ball->radius));
        DeleteObject(brush);
    }
}
