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

int main(int argc, char* argv[])
{


	WSADATA wsa_data;
	int i_result;
	i_result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if(i_result != 0)
	{
		printf("WSAStartup failed: %d\n", i_result);
		return 1;
	}

	struct addrinfo *result = NULL,
									*ptr = NULL,
									hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	i_result = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	printf("%d\n",i_result);
	printf("%ld\n", WSAGetLastError());
	if(i_result != 0) {
		printf("error in connection");
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
		printf("Unable to connect");
		WSACleanup();
		return 1;
	}

	int recv_buf_len = DEFAULT_BUFFER_LENGTH;
	const char *send_buf = "test";
	char recv_buf[DEFAULT_BUFFER_LENGTH];

	i_result = send(ConnectSocket, send_buf, (int)strlen(send_buf), 0);
	if(i_result == SOCKET_ERROR){
		printf("send failed");
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	printf("bytes sent: %ld\n", i_result);

	i_result = shutdown(ConnectSocket, SD_SEND);
	if(i_result == SOCKET_ERROR){
		printf("shotdown failed");
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	do{
		i_result = recv(ConnectSocket, recv_buf, recv_buf_len, 0);
		if(i_result > 0)
			printf("bytes received: %d\n", i_result);
		else if(i_result == 0)
			printf("connection closed\n");
		else
			printf("recv failed");
	}
	while(i_result > 0);
}
