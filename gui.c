#include <windows.h>

HWND hwndButton;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CREATE:
            hwndButton = CreateWindow(
                "BUTTON",  
                "Click",
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 
                10,         
                10,        
                100,      
                30,      
                hwnd,    
                (HMENU)1,
                (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
                NULL);   
            break;
        case WM_COMMAND:
            if (LOWORD(wParam) == 1) { 
                MessageBox(hwnd, "Message", "Hello World", MB_OK | MB_ICONINFORMATION);
            }
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    const char CLASS_NAME[]  = "Sample Window Class";
    
    WNDCLASS wc = { };

    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,                              
        CLASS_NAME,                     
        "Learn to Program Windows",     
        WS_OVERLAPPEDWINDOW,            

        CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,

        NULL,       
        NULL,       
        hInstance,  
        NULL        
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
