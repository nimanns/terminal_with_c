#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8081
#define BUFFER_SIZE 1024

void generate_html(char *buffer) {
    char *html = "<html><body><h1>Hello, World!</h1><p>This is a simple server-side generated page.</p></body></html>";
    strcpy(buffer, html);
}

int main() {
    WSADATA wsa_data;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE] = {0};

    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Listen failed: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len)) == INVALID_SOCKET) {
            printf("Accept failed: %d\n", WSAGetLastError());
            closesocket(server_socket);
            WSACleanup();
            return 1;
        }

        generate_html(buffer);

        char http_header[BUFFER_SIZE];
        sprintf(http_header, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n", (int)strlen(buffer));

        send(client_socket, http_header, (int)strlen(http_header), 0);
        send(client_socket, buffer, (int)strlen(buffer), 0);

        closesocket(client_socket);
    }

    closesocket(server_socket);
    WSACleanup();

    return 0;
}
