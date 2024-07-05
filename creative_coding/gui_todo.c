#include <windows.h>
#include <stdlib.h>
#include <string.h>

#define ID_LISTBOX 1
#define ID_EDIT 2
#define ID_ADD_BUTTON 3
#define ID_REMOVE_BUTTON 4

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
void AddControlsToWindow(HWND);
void AddItem(HWND);
void RemoveItem(HWND);

HWND hListBox, hEdit, hAddButton, hRemoveButton;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
    WNDCLASSW wc = {0};
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = hInst;
    wc.lpszClassName = L"ToDoListClass";
    wc.lpfnWndProc = WindowProcedure;

    if (!RegisterClassW(&wc))
        return -1;

    CreateWindowW(L"ToDoListClass", L"To-Do List", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                  100, 100, 400, 500, NULL, NULL, NULL, NULL);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_CREATE:
        AddControlsToWindow(hWnd);
        break;
    case WM_COMMAND:
        switch (wp)
        {
        case ID_ADD_BUTTON:
            AddItem(hWnd);
            break;
        case ID_REMOVE_BUTTON:
            RemoveItem(hWnd);
            break;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hWnd, msg, wp, lp);
    }
    return 0;
}

void AddControlsToWindow(HWND hWnd)
{
    hListBox = CreateWindowW(L"LISTBOX", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | LBS_NOTIFY,
                             20, 20, 340, 300, hWnd, (HMENU)ID_LISTBOX, NULL, NULL);

    hEdit = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER,
                          20, 330, 340, 30, hWnd, (HMENU)ID_EDIT, NULL, NULL);

    hAddButton = CreateWindowW(L"BUTTON", L"Add", WS_VISIBLE | WS_CHILD,
                               20, 370, 100, 30, hWnd, (HMENU)ID_ADD_BUTTON, NULL, NULL);

    hRemoveButton = CreateWindowW(L"BUTTON", L"Remove", WS_VISIBLE | WS_CHILD,
                                  130, 370, 100, 30, hWnd, (HMENU)ID_REMOVE_BUTTON, NULL, NULL);
}

void AddItem(HWND hWnd)
{
    char item[256];
    GetWindowTextA(hEdit, item, sizeof(item));

    if (strlen(item) > 0)
    {
        SendMessageA(hListBox, LB_ADDSTRING, 0, (LPARAM)item);
        SetWindowTextA(hEdit, "");
    }
}

void RemoveItem(HWND hWnd)
{
    int selectedIndex = SendMessage(hListBox, LB_GETCURSEL, 0, 0);
    if (selectedIndex != LB_ERR)
    {
        SendMessage(hListBox, LB_DELETESTRING, selectedIndex, 0);
    }
}
