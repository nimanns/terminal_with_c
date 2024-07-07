#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <commdlg.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "comdlg32.lib")

HWND hPlayButton, hStopButton, hBrowseButton;
char szFileName[MAX_PATH] = "";

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    static TCHAR szAppName[] = TEXT("WAV Player");
    HWND hwnd;
    MSG msg;
    WNDCLASS wndclass;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass(&wndclass)) {
        MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
        return 0;
    }

    hwnd = CreateWindow(szAppName, TEXT("WAV Player"),
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        300, 200,
                        NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE:
            hBrowseButton = CreateWindow("BUTTON", "Browse", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                         10, 10, 100, 30, hwnd, (HMENU)1, NULL, NULL);
            hPlayButton = CreateWindow("BUTTON", "Play", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                       10, 50, 100, 30, hwnd, (HMENU)2, NULL, NULL);
            hStopButton = CreateWindow("BUTTON", "Stop", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                       120, 50, 100, 30, hwnd, (HMENU)3, NULL, NULL);
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case 1:
                    {
                        OPENFILENAME ofn;
                        ZeroMemory(&ofn, sizeof(ofn));
                        ofn.lStructSize = sizeof(ofn);
                        ofn.hwndOwner = hwnd;
                        ofn.lpstrFilter = "WAV Files (*.wav)\0*.wav\0All Files (*.*)\0*.*\0";
                        ofn.lpstrFile = szFileName;
                        ofn.nMaxFile = MAX_PATH;
                        ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
                        ofn.lpstrDefExt = "wav";

                        if (GetOpenFileName(&ofn)) {
                            MessageBox(hwnd, szFileName, "File Selected", MB_OK);
                        } else {
														MessageBox(hwnd, "No files selected.", "No files selected", MB_OK);
												}
                    }
                    break;
                case 2:
                    if (strlen(szFileName) > 0) {
                        if (!PlaySound(szFileName, NULL, SND_FILENAME | SND_ASYNC)) {
                            MessageBox(hwnd, "Error playing the file", "Error", MB_OK | MB_ICONERROR);
														printf("error 1");
                        }
                    } else {
                        MessageBox(hwnd, "Please select a WAV file first", "Error", MB_OK | MB_ICONINFORMATION);
												printf("error 2");
                    }
                    break;
                case 3: 
                    PlaySound(NULL, NULL, 0);
                    break;
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}
