#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFFER_LENGTH 512


int main(void)
{
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
	hints.ai_flags = AI_PASSIVE;
	i_result = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if(i_result != 0)
	{
		printf("getaddrinfo failed: %d\n", i_result);
		WSACleanup();
		return 1;
	}

	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
    printf("Error at socket(): %ld\n", WSAGetLastError());
    freeaddrinfo(result);
    WSACleanup();
    return 1;
	}

	i_result = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);

	if (i_result == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
	}
	printf("listening on 127.0.0.1:%d", DEFAULT_PORT);

	freeaddrinfo(result);

	if(listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	SOCKET ClientSocket;
	ClientSocket = INVALID_SOCKET;
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("failed: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	
	char recv_buf[DEFAULT_BUFFER_LENGTH];
	int i_send_result;
	int recv_buf_len = DEFAULT_BUFFER_LENGTH;

	do{
		i_result = recv(ClientSocket, recv_buf, recv_buf_len, 0);
		if(i_result > 0){
			printf("Bytes Received: %d\n", i_result);

			i_send_result = send(ClientSocket, recv_buf, i_result, 0);
			if(i_send_result == SOCKET_ERROR){
				printf("send error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			printf("Bytes Sent: %d\n", i_send_result);
		} else if (i_result == 0)
				printf("connection closing...\n");
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}
	} while(i_result>0);
	return 0;
}
