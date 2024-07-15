#include <windows.h>

#define TOGGLE_CLASS "custom_toggle"
#define TOGGLE_WIDTH 60
#define TOGGLE_HEIGHT 30
#define CIRCLE_DIAMETER 26

typedef struct {
    BOOL is_on;
    HBRUSH on_brush;
    HBRUSH off_brush;
    HBRUSH circle_brush;
} toggle_info;

LRESULT CALLBACK toggle_procedure(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) {
    toggle_info* p_info = (toggle_info*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg) {
        case WM_CREATE:
        {
            p_info = (toggle_info*)malloc(sizeof(toggle_info));
            p_info->is_on = FALSE;
            p_info->on_brush = CreateSolidBrush(RGB(0, 255, 0));
            p_info->off_brush = CreateSolidBrush(RGB(200, 200, 200));
            p_info->circle_brush = CreateSolidBrush(RGB(255, 255, 255));
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)p_info);
            return 0;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            RECT rect;
            GetClientRect(hwnd, &rect);

            FillRect(hdc, &rect, p_info->is_on ? p_info->on_brush : p_info->off_brush);

            int circle_x = p_info->is_on ? rect.right - CIRCLE_DIAMETER - 2 : rect.left + 2;
            int circle_y = (rect.bottom - CIRCLE_DIAMETER) / 2;

            HGDIOBJ old_brush = SelectObject(hdc, p_info->circle_brush);
            Ellipse(hdc, circle_x, circle_y, circle_x + CIRCLE_DIAMETER, circle_y + CIRCLE_DIAMETER);
            SelectObject(hdc, old_brush);

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_LBUTTONDOWN:
        {
            p_info->is_on = !p_info->is_on;
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }

        case WM_DESTROY:
        {
            DeleteObject(p_info->on_brush);
            DeleteObject(p_info->off_brush);
            DeleteObject(p_info->circle_brush);
            free(p_info);
            return 0;
        }
    }

    return DefWindowProc(hwnd, msg, w_param, l_param);
}

BOOL register_toggle_class(HINSTANCE h_instance) {
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = toggle_procedure;
    wc.hInstance = h_instance;
    wc.lpszClassName = TOGGLE_CLASS;
    wc.hCursor = LoadCursor(NULL, IDC_HAND);

    return RegisterClassA(&wc);
}

int WINAPI WinMain(HINSTANCE h_instance, HINSTANCE h_prev_instance, LPSTR lp_cmd_line, int n_cmd_show) {
    register_toggle_class(h_instance);

    HWND hwnd = CreateWindowExA(
        0,
        "STATIC",
        "Toggle Switch Example",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
        NULL, NULL, h_instance, NULL
    );

    CreateWindowExA(
        0,
        TOGGLE_CLASS,
        NULL,
        WS_CHILD | WS_VISIBLE,
        100, 80, TOGGLE_WIDTH, TOGGLE_HEIGHT,
        hwnd, NULL, h_instance, NULL
    );

    ShowWindow(hwnd, n_cmd_show);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
