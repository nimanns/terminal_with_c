#include <windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "simple paint";
    
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_CROSS);
    
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,                          
        CLASS_NAME,                 
        "simple paint",
        WS_OVERLAPPEDWINDOW,
        
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        
        NULL,       
        NULL,       
        hInstance,  
        NULL        
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static BOOL fDrawing = FALSE;
    static POINT ptPrevious = {0};

    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_LBUTTONDOWN:
            fDrawing = TRUE;
            ptPrevious.x = LOWORD(lParam);
            ptPrevious.y = HIWORD(lParam);
            return 0;

        case WM_LBUTTONUP:
            fDrawing = FALSE;
            return 0;

        case WM_MOUSEMOVE:
            if (fDrawing) {
                HDC hdc = GetDC(hwnd);
                MoveToEx(hdc, ptPrevious.x, ptPrevious.y, NULL);
                LineTo(hdc, LOWORD(lParam), HIWORD(lParam));
                ReleaseDC(hwnd, hdc);
                ptPrevious.x = LOWORD(lParam);
                ptPrevious.y = HIWORD(lParam);
            }
            return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
