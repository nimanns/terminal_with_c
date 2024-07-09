#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma comment(lib, "ws2_32")
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFFER_LENGTH 512

DWORD WINAPI ReceiveThread(LPVOID lpParam);

int main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("Usage: %s server_ip\n", argv[0]);
        return 1;
    }

    WSADATA wsa_data;
    int i_result;
    i_result = WSAStartup(MAKEWORD(2,2), &wsa_data);
    if(i_result != 0)
    {
        printf("WSAStartup failed: %d\n", i_result);
        return 1;
    }

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

		//thread for message receiving
    HANDLE h_thread = CreateThread(NULL, 0, ReceiveThread, (LPVOID)ConnectSocket, 0, NULL);
    if (h_thread == NULL) {
        printf("failed to create receive thread\n");
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    char send_buf[DEFAULT_BUFFER_LENGTH];
    while(1){
        printf("Enter message (or 'quit' to exit): ");
        fgets(send_buf, DEFAULT_BUFFER_LENGTH, stdin);
        send_buf[strcspn(send_buf, "\n")] = 0;

        if (strcmp(send_buf, "quit") == 0) {
            break;
        }

        i_result = send(ConnectSocket, send_buf, (int)strlen(send_buf), 0);
        if(i_result == SOCKET_ERROR){
            printf("send failed: %d\n", WSAGetLastError());
            break;
        }
    }

    closesocket(ConnectSocket);
    WSACleanup();
    return 0;
}

DWORD WINAPI ReceiveThread(LPVOID lpParam)
{
    SOCKET ConnectSocket = (SOCKET)lpParam;
    char recv_buf[DEFAULT_BUFFER_LENGTH];
    int i_result;

    while(1){
        i_result = recv(ConnectSocket, recv_buf, DEFAULT_BUFFER_LENGTH, 0);
        if(i_result > 0){
            recv_buf[i_result] = '\0';
            printf("%s\n", recv_buf);
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
