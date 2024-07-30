#include <windows.h>
#include <stdlib.h>
#include <time.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define BALL_SIZE 30

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

typedef struct {
    int x, y;
    int dx, dy;
    COLORREF color;
} Ball;

Ball ball;

void init_ball() {
    ball.x = rand() % (WINDOW_WIDTH - BALL_SIZE);
    ball.y = rand() % (WINDOW_HEIGHT - BALL_SIZE);
    ball.dx = (rand() % 5) + 1;
    ball.dy = (rand() % 5) + 1;
    ball.color = RGB(rand() % 256, rand() % 256, rand() % 256);
}

void update_ball() {
    ball.x += ball.dx;
    ball.y += ball.dy;

    if (ball.x <= 0 || ball.x >= WINDOW_WIDTH - BALL_SIZE) {
        ball.dx = -ball.dx;
        ball.color = RGB(rand() % 256, rand() % 256, rand() % 256);
    }

    if (ball.y <= 0 || ball.y >= WINDOW_HEIGHT - BALL_SIZE) {
        ball.dy = -ball.dy;
        ball.color = RGB(rand() % 256, rand() % 256, rand() % 256);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    srand(time(NULL));

    const char CLASS_NAME[] = "Bouncing Ball Window Class";

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Bouncing Ball",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        return 0;
    }
		SetTimer(hwnd, 1, 16, NULL);
    ShowWindow(hwnd, nCmdShow);

    init_ball();

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
        PostQuitMessage(0);
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
				
				RECT background_rect;
				GetClientRect(hwnd, &background_rect);
				HBRUSH background_brush = CreateSolidBrush(RGB(255,255,255));
				FillRect(hdc, &background_rect, background_brush);
				DeleteObject(background_brush);

        HBRUSH brush = CreateSolidBrush(ball.color);
        SelectObject(hdc, brush);

        Ellipse(hdc, ball.x, ball.y, ball.x + BALL_SIZE, ball.y + BALL_SIZE);

        DeleteObject(brush);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_TIMER:
        update_ball();
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
