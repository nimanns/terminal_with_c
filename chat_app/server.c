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
#define MAX_CLIENTS 10

struct Client {
	SOCKET socket;
	const char* name;
};

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
    struct addrinfo *result = NULL, hints;
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
    SOCKET ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
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
    freeaddrinfo(result);
    
    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Listen failed with error: %ld\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    printf("Server is listening on port %s\n", DEFAULT_PORT);

    fd_set master_set, read_set;
    struct Client clients[MAX_CLIENTS];
		const char* names[MAX_CLIENTS] = {"User 1", "User 2", "User 3", "User 4", "User 5", "User 6", "User 7", "User 8", "User 9", "User 10"};
    int client_count = 0;

    FD_ZERO(&master_set);
    FD_SET(ListenSocket, &master_set);

    while(1) {
        read_set = master_set;
        if (select(0, &read_set, NULL, NULL, NULL) == SOCKET_ERROR) {
            printf("select failed with error: %d\n", WSAGetLastError());
            break;
        }

        for (int i = 0; i < master_set.fd_count; i++) {
            SOCKET current_socket = master_set.fd_array[i];
            if (FD_ISSET(current_socket, &read_set)) {
                if (current_socket == ListenSocket) {
                    SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
                    if (ClientSocket == INVALID_SOCKET) {
                        printf("accept failed: %d\n", WSAGetLastError());
                    } else {
                        if (client_count < MAX_CLIENTS) {
                            FD_SET(ClientSocket, &master_set);
														clients[client_count].name = names[client_count];
                            clients[client_count].socket = ClientSocket;
                            printf("%s connected\n", clients[client_count].name);
														client_count++;
                        } else {
                            printf("Max clients reached. Connection rejected.\n");
                            closesocket(ClientSocket);
                        }
                    }
                } else {
                    char recv_buf[DEFAULT_BUFFER_LENGTH];
                    int recv_result = recv(current_socket, recv_buf, DEFAULT_BUFFER_LENGTH, 0);
                    if (recv_result > 0) {
                        printf("Bytes received: %d\n", recv_result);
                        for (int j = 0; j < client_count; j++) {
                            if (clients[j].socket != current_socket) {
																char message_with_name[DEFAULT_BUFFER_LENGTH + 200];
																strcat(message_with_name, clients[j].name);
																strcat(message_with_name, " said: ");
																strcat(message_with_name, recv_buf);
                                int send_result = send(clients[j].socket, message_with_name, strlen(message_with_name), 0);
                                if (send_result == SOCKET_ERROR) {
                                    printf("send failed: %d\n", WSAGetLastError());
                                }
																memset(message_with_name, 0, strlen(message_with_name));
                            }
                        }
                    } else if (recv_result == 0) {
                        printf("Connection closing...\n");
                        closesocket(current_socket);
                        FD_CLR(current_socket, &master_set);
                        for (int j = 0; j < client_count; j++) {
                            if (clients[j].socket == current_socket) {
                                for (int k = j; k < client_count - 1; k++) {
                                    clients[k] = clients[k + 1];
                                }
                                client_count--;
                                break;
                            }
                        }
                    } else {
                        printf("recv failed: %d\n", WSAGetLastError());
                        closesocket(current_socket);
                        FD_CLR(current_socket, &master_set);
                        for (int j = 0; j < client_count; j++) {
                            if (clients[j].socket == current_socket) {
                                for (int k = j; k < client_count - 1; k++) {
                                    clients[k] = clients[k + 1];
                                }
                                client_count--;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i < client_count; i++) {
        closesocket(clients[i].socket);
    }
    closesocket(ListenSocket);
    WSACleanup();
    return 0;
}
