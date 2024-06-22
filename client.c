#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    struct sockaddr_in serv_addr;
    int sock = 0;
    char buffer[BUFFER_SIZE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\nSocket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported\n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed\n");
        return -1;
    }

    printf("Connected to server on port %d\n", PORT);

    while (1) {
        printf("Client: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        send(sock, buffer, strlen(buffer), 0);
        if (strcmp(buffer, "exit") == 0) {
            printf("Closing connection...\n");
            break;
        }
        read(sock, buffer, BUFFER_SIZE);
        printf("Server: %s\n", buffer);
    }

    close(sock);
    return 0;
}
