#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define ID_LISTBOX 1
#define ID_EDIT 2
#define ID_ADD_BUTTON 3
#define ID_REMOVE_BUTTON 4
#define MAX_ITEM_LENGTH 256
#define FILENAME "todo_list.txt"

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
void AddControlsToWindow(HWND);
void AddItem(HWND);
void RemoveItem(HWND);
void SaveList(HWND);
void LoadList(HWND);

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

    HWND hWnd = CreateWindowExW(
        0,
        L"ToDoListClass",
        L"To-Do List",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
        (GetSystemMetrics(SM_CXSCREEN) / 2) - 200, (GetSystemMetrics(SM_CYSCREEN) / 2) - 250, 400, 500,
        NULL, NULL, hInst, NULL
    );

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
        LoadList(hWnd);
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
    case WM_CLOSE:
        SaveList(hWnd);
        DestroyWindow(hWnd);
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
    HINSTANCE hInst = GetModuleHandle(NULL);

    hListBox = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"LISTBOX", L"",
        WS_CHILD | WS_VISIBLE | LBS_NOTIFY,
        20, 20, 340, 300,
        hWnd, (HMENU)ID_LISTBOX, hInst, NULL
    );

    hEdit = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"EDIT", L"",
        WS_CHILD | WS_VISIBLE,
        20, 330, 340, 30,
        hWnd, (HMENU)ID_EDIT, hInst, NULL
    );

    hAddButton = CreateWindowExW(
        0,
        L"BUTTON", L"Add",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        20, 370, 100, 30,
        hWnd, (HMENU)ID_ADD_BUTTON, hInst, NULL
    );

    hRemoveButton = CreateWindowExW(
        0,
        L"BUTTON", L"Remove",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        130, 370, 100, 30,
        hWnd, (HMENU)ID_REMOVE_BUTTON, hInst, NULL
    );
}

void AddItem(HWND hWnd)
{
    char item[MAX_ITEM_LENGTH];
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

void SaveList(HWND hWnd)
{
    FILE *file = fopen(FILENAME, "w");
    if (file == NULL)
    {
        MessageBoxW(hWnd, L"Error saving list", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    int count = SendMessage(hListBox, LB_GETCOUNT, 0, 0);
    for (int i = 0; i < count; i++)
    {
        char item[MAX_ITEM_LENGTH];
        SendMessageA(hListBox, LB_GETTEXT, i, (LPARAM)item);
        fprintf(file, "%s\n", item);
    }

    fclose(file);
}

void LoadList(HWND hWnd)
{
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL)
    {
        return;
    }

    char item[MAX_ITEM_LENGTH];
    while (fgets(item, sizeof(item), file))
    {
        item[strcspn(item, "\n")] = 0;
        SendMessageA(hListBox, LB_ADDSTRING, 0, (LPARAM)item);
    }

    fclose(file);
}
