#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef UNICODE
#define UNICODE
#endif


#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#pragma comment(lib, "ws2_32")
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFFER_LENGTH 512
#define IDC_MESSAGE_TEXT 101
#define IDC_SEND_BUTTON 102
#define IDC_CHAT_BOX 103
#define WM_USER_DATA (WM_USER + 1)

DWORD WINAPI ReceiveThread(LPVOID lpParam);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
char* WideCharToChar(const WCHAR* wideString);
WCHAR* CharToWideChar(const char* narrowString);
HWND g_hwnd;

WCHAR message_thread[3072];
int send_message = 0;
CRITICAL_SECTION g_cs;
char* g_shared_data = NULL;
bool g_data_ready = false;

struct ThreadParams {
    SOCKET ConnectSocket;
    HWND hwnd;
};

int WINAPI wWinMain(HINSTANCE h_instance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
		const wchar_t CLASS_NAME[] = L"Simple Chat App";

		WNDCLASS wc = {};

		wc.lpfnWndProc = WindowProc;
		wc.hInstance = h_instance;
		wc.lpszClassName = CLASS_NAME;

		RegisterClass(&wc);

		HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"Chat App", WS_OVERLAPPEDWINDOW, 100, 100, 500, 900, NULL, NULL, h_instance, NULL);

		if(hwnd == NULL) 
		{
			return 0;
		}

		g_hwnd = hwnd;

		const wchar_t EDITOR_CLASS[] = L"Editor";

		HWND h_chat_box = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", message_thread, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOVSCROLL | ES_MULTILINE | WS_VSCROLL | ES_READONLY, 10, 10, 460, 480, hwnd, (HMENU)IDC_CHAT_BOX, h_instance, NULL);

		HWND h_message_box = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"TYPE YOUR MESSAGE HERE", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOVSCROLL | ES_MULTILINE | WS_VSCROLL, 10, 480, 460, 200, hwnd, (HMENU)IDC_MESSAGE_TEXT, h_instance, NULL);
		ShowWindow(hwnd, nCmdShow);
		UpdateWindow(hwnd);

		HWND h_send_button = CreateWindow(L"BUTTON", L"Send Message", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 690, 460, 50, hwnd, (HMENU)IDC_SEND_BUTTON, h_instance, NULL);
		
		MSG msg = {};

		int message_length = GetWindowTextLength(h_message_box);
		WCHAR* buffer = (WCHAR*)malloc((message_length + 1) * sizeof(WCHAR));

				
		while (GetMessage(&msg, NULL, 0, 0) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return 0;
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	switch(uMsg)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hwnd, &ps);

				FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));

				EndPaint(hwnd, &ps);
			}
			return 0;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_SEND_BUTTON:
					
					HWND h_message_box = GetDlgItem(hwnd, IDC_MESSAGE_TEXT);
					HWND h_chat_box = GetDlgItem(hwnd, IDC_CHAT_BOX);
					
					GetWindowTextW(h_chat_box, message_thread, 3071);
					
					
					WCHAR buff[1024];
					GetWindowTextW(h_message_box, buff, 1024);
					if(buff[0] == L'\0'){
						MessageBoxW(hwnd, L"Please type a message!", L"Error", MB_OK);
						break;
					}
					
					if (wcslen(message_thread) + wcslen(buff) < 3071) {
							wcscat_s(message_thread, 3072, buff);
							wcscat_s(message_thread, 3072, L"\r\n");
							SetWindowTextW(h_chat_box, message_thread);
							SetWindowTextW(h_message_box, L"");
					}
					else {
							MessageBoxW(hwnd, L"Message too long!", L"Error", MB_OK);
					}
					send_message = 1;

					EnterCriticalSection(&g_cs);
					if(g_shared_data) free(g_shared_data);
					g_shared_data = _strdup(WideCharToChar(buff));
					g_data_ready = true;
					LeaveCriticalSection(&g_cs);
					break;
				}
				break;

			case WM_USER_DATA:
				{
					WCHAR* new_message = (WCHAR*)lParam;
					HWND h_chat_box = GetDlgItem(hwnd, IDC_CHAT_BOX);

					if(wcslen(message_thread) + wcslen(new_message) < 3071){
						wcscat_s(message_thread, 3072, new_message);
						wcscat_s(message_thread, 3072, L"\r\n");
						SetWindowTextW(h_chat_box, message_thread);
					} else {
						MessageBoxW(hwnd, L"Chat history too long!!", L"Error", MB_OK);
						wcscpy_s(message_thread, 3072, new_message);
						wcscat_s(message_thread, 3072, L"\r\n");
						SetWindowTextW(h_chat_box, message_thread);
					}
					free(new_message);
				}
				return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

DWORD WINAPI WindowThread(LPVOID lpParam){
	HINSTANCE h_instance = GetModuleHandle(NULL);
	wWinMain(h_instance, NULL, GetCommandLineW(), SW_SHOW);
	return 0;
}

int main(int argc, char* argv[])
{
		HANDLE hWindowThread = CreateThread(NULL, 0, WindowThread, NULL, 0, NULL);
		if(hWindowThread == NULL) {
			printf("failed to create the window\n");
			return 1;
		}
		
		//WaitForSingleObject(hWindowThread, INFINITE);
		//CloseHandle(hWindowThread);

		if (argc != 2) {
        printf("Usage: %s server_ip\n", argv[0]);
        return 1;
    }
		
		InitializeCriticalSection(&g_cs);
			
		while (g_hwnd == NULL) {
        Sleep(100);
    }

    WSADATA wsa_data;
    int i_result;
    i_result = WSAStartup(MAKEWORD(2,2), &wsa_data);
    if(i_result != 0)
    {
        printf("WSAStartup failed: %d\n", i_result);
        return 1;
    }

		MSG msg;

    struct addrinfo *result = NULL, *ptr = NULL, hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

		if(strcmp(argv[1], "local") == 0){
			i_result = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
		} else {
			i_result = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
		}

    if(i_result != 0) {
        printf("getaddrinfo failed: %d\n", i_result);
        WSACleanup();
        return 1;
    }

    SOCKET ConnectSocket = INVALID_SOCKET;
    ptr = result;
    
    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    i_result = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if(i_result == SOCKET_ERROR){
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }

    freeaddrinfo(result);

    if(ConnectSocket == INVALID_SOCKET){
        printf("unable to connect to server\n");
        WSACleanup();
        return 1;
    }

    printf("Connected to server\n");

		struct ThreadParams params;
		params.ConnectSocket = ConnectSocket;
		params.hwnd = g_hwnd;
		//thread for message receiving
    HANDLE h_thread = CreateThread(NULL, 0, ReceiveThread, (LPVOID)&params, 0, NULL);
    if (h_thread == NULL) {
        printf("failed to create receive thread\n");
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    char send_buf[DEFAULT_BUFFER_LENGTH];
    while(1){
        //printf("Enter message (or 'quit' to exit): ");
        //fgets(send_buf, DEFAULT_BUFFER_LENGTH, stdin);
        
				while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
					TranslateMessage(&msg);
					DispatchMessage(&msg);

					if(msg.message == WM_QUIT){
						goto cleanup;
					}
				}

				EnterCriticalSection(&g_cs);
				if(g_data_ready){
					if(g_shared_data){
						strcpy(send_buf, g_shared_data);
						send_buf[strcspn(send_buf, "\n")] = 0;
						
						if (strcmp(send_buf, "quit") == 0) {
								break;
						}

						i_result = send(ConnectSocket, send_buf, (int)strlen(send_buf), 0);
						if(i_result == SOCKET_ERROR){
								printf("send failed: %d\n", WSAGetLastError());
								break;
						}
						free(g_shared_data);
						g_shared_data = NULL;
					}
					g_data_ready = false;
				}
				LeaveCriticalSection(&g_cs);

    }

    closesocket(ConnectSocket);
    WSACleanup();
    
		WaitForSingleObject(hWindowThread, INFINITE);
		CloseHandle(hWindowThread);
cleanup:
		DeleteCriticalSection(&g_cs);
		return 0;
}

DWORD WINAPI ReceiveThread(LPVOID lpParam)
{
		struct ThreadParams* params = (struct ThreadParams*)lpParam;
    SOCKET ConnectSocket = params->ConnectSocket;
		HWND hwnd = params->hwnd;

    char recv_buf[DEFAULT_BUFFER_LENGTH];
    int i_result;

    while(1){
        i_result = recv(ConnectSocket, recv_buf, DEFAULT_BUFFER_LENGTH, 0);
        if(i_result > 0){
            recv_buf[i_result] = '\0';
            printf("%s\n", recv_buf);
						
						WCHAR* wide_msg = CharToWideChar(recv_buf);
						if(wide_msg){
							PostMessage(hwnd, WM_USER_DATA, 0, (LPARAM)wide_msg);
						}

        }
        else if(i_result == 0){
            printf("Server closed the connection\n");
            break;
        }
        else {
            printf("recv failed: %d\n", WSAGetLastError());
            break;
        }
    }
		
		return 0;
}

char* WideCharToChar(const WCHAR* wideString) {
    int bufferSize;
    char* narrowString;

    bufferSize = WideCharToMultiByte(CP_UTF8, 0, wideString, -1, NULL, 0, NULL, NULL);
    if (bufferSize == 0) {
        return NULL;
    }

    narrowString = (char*)malloc(bufferSize);
    if (narrowString == NULL) {
        return NULL;
    }

    if (WideCharToMultiByte(CP_UTF8, 0, wideString, -1, narrowString, bufferSize, NULL, NULL) == 0) {
        free(narrowString);
        return NULL;
    }

    return narrowString;
}

WCHAR* CharToWideChar(const char* narrowString) {
    int bufferSize;
    WCHAR* wideString;

    // Get the required buffer size
    bufferSize = MultiByteToWideChar(CP_UTF8, 0, narrowString, -1, NULL, 0);
    if (bufferSize == 0) {
        // Handle error
        return NULL;
    }

    // Allocate the buffer
    wideString = (WCHAR*)malloc(bufferSize * sizeof(WCHAR));
    if (wideString == NULL) {
        // Handle memory allocation failure
        return NULL;
    }

    // Perform the conversion
    if (MultiByteToWideChar(CP_UTF8, 0, narrowString, -1, wideString, bufferSize) == 0) {
        // Handle error
        free(wideString);
        return NULL;
    }

    return wideString;
}
