#include <windows.h>
#include <dshow.h>
#include <commdlg.h>

#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "comdlg32.lib")

//only plays wmv files for now
//

HWND hwnd;
IGraphBuilder *pGraph = NULL;
IMediaControl *pControl = NULL;
IMediaEvent *pEvent = NULL;
IVideoWindow *pVidWin = NULL;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

BOOL OpenVideoFile(HWND hwnd, LPWSTR szFileName, DWORD nMaxFileName)
{
    OPENFILENAMEW ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = L"Video Files\0*.avi;*.mp4;*.mkv\0All Files\0*.*\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = nMaxFileName;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = L"mp4";
    return GetOpenFileNameW(&ofn);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wcex;
    MSG msg;
    TCHAR szWindowClass[] = TEXT("VideoPlayerClass");
    TCHAR szTitle[] = TEXT("Video Player");

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL, TEXT("Call to RegisterClassEx failed!"), TEXT("Video Player"), MB_ICONERROR);
        return 1;
    }

    hwnd = CreateWindow(
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hwnd)
    {
        MessageBox(NULL, TEXT("Call to CreateWindow failed!"), TEXT("Video Player"), MB_ICONERROR);
        return 1;
    }

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr))
    {
        MessageBox(NULL, TEXT("Failed to initialize COM"), TEXT("Error"), MB_ICONERROR);
        return 1;
    }

    hr = CoCreateInstance(&CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
                     &IID_IGraphBuilder, (void **)&pGraph);
    if (FAILED(hr))
    {
        MessageBox(NULL, TEXT("Failed to create Filter Graph Manager"), TEXT("Error"), MB_ICONERROR);
        CoUninitialize();
        return 1;
    }

    hr = pGraph->lpVtbl->QueryInterface(pGraph, &IID_IMediaControl, (void **)&pControl);
    if (FAILED(hr))
    {
        MessageBox(NULL, TEXT("Failed to get MediaControl interface"), TEXT("Error"), MB_ICONERROR);
        pGraph->lpVtbl->Release(pGraph);
        CoUninitialize();
        return 1;
    }

    hr = pGraph->lpVtbl->QueryInterface(pGraph, &IID_IMediaEvent, (void **)&pEvent);
    if (FAILED(hr))
    {
        MessageBox(NULL, TEXT("Failed to get MediaEvent interface"), TEXT("Error"), MB_ICONERROR);
        pControl->lpVtbl->Release(pControl);
        pGraph->lpVtbl->Release(pGraph);
        CoUninitialize();
        return 1;
    }

    WCHAR szFileName[MAX_PATH] = {0};
    if (!OpenVideoFile(hwnd, szFileName, MAX_PATH))
    {
        MessageBox(NULL, TEXT("No file selected"), TEXT("Information"), MB_ICONINFORMATION);
        pEvent->lpVtbl->Release(pEvent);
        pControl->lpVtbl->Release(pControl);
        pGraph->lpVtbl->Release(pGraph);
        CoUninitialize();
        return 1;
    }

    hr = pGraph->lpVtbl->RenderFile(pGraph, szFileName, NULL);
    if (FAILED(hr))
    {
        MessageBox(NULL, TEXT("Failed to render file"), TEXT("Error"), MB_ICONERROR);
        pEvent->lpVtbl->Release(pEvent);
        pControl->lpVtbl->Release(pControl);
        pGraph->lpVtbl->Release(pGraph);
        CoUninitialize();
        return 1;
    }

    hr = pGraph->lpVtbl->QueryInterface(pGraph, &IID_IVideoWindow, (void **)&pVidWin);
    if (FAILED(hr))
    {
        MessageBox(NULL, TEXT("Failed to get VideoWindow interface"), TEXT("Error"), MB_ICONERROR);
        pEvent->lpVtbl->Release(pEvent);
        pControl->lpVtbl->Release(pControl);
        pGraph->lpVtbl->Release(pGraph);
        CoUninitialize();
        return 1;
    }

    pVidWin->lpVtbl->put_Owner(pVidWin, (OAHWND)hwnd);
    pVidWin->lpVtbl->put_WindowStyle(pVidWin, WS_CHILD | WS_CLIPSIBLINGS);

    RECT rcClient;
    GetClientRect(hwnd, &rcClient);
    pVidWin->lpVtbl->SetWindowPosition(pVidWin, 0, 0, rcClient.right, rcClient.bottom);

    hr = pControl->lpVtbl->Run(pControl);
    if (FAILED(hr))
    {
        MessageBox(NULL, TEXT("Failed to run the graph"), TEXT("Error"), MB_ICONERROR);
        pVidWin->lpVtbl->Release(pVidWin);
        pEvent->lpVtbl->Release(pEvent);
        pControl->lpVtbl->Release(pControl);
        pGraph->lpVtbl->Release(pGraph);
        CoUninitialize();
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    pControl->lpVtbl->Stop(pControl);
    pVidWin->lpVtbl->put_Visible(pVidWin, OAFALSE);
    pVidWin->lpVtbl->put_Owner(pVidWin, (OAHWND)NULL);
    pVidWin->lpVtbl->Release(pVidWin);
    pEvent->lpVtbl->Release(pEvent);
    pControl->lpVtbl->Release(pControl);
    pGraph->lpVtbl->Release(pGraph);
    CoUninitialize();

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
