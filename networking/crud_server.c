#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define FILENAME "posts.json"

struct Post {
    int userId;
    int id;
    char title[100];
    char body[1000];
};

void createPost(const char *filename, struct Post *post) {
    FILE *fp = fopen(filename, "a");
    if (fp == NULL) {
        perror("Error opening file");
        return;
    }
    fprintf(fp, "{ \"userId\": %d, \"id\": %d, \"title\": \"%s\", \"body\": \"%s\" }\n",
            post->userId, post->id, post->title, post->body);
    fclose(fp);
}

void readPosts(const char *filename) {
    FILE *fp = fopen(filename, "r");
    char line[1024];

    if (fp == NULL) {
        perror("Error opening file");
        return;
    }

    while (fgets(line, sizeof(line), fp)) {
        printf("Data: %s", line);
    }

    fclose(fp);
}

void updatePost(const char *filename, int index, struct Post *post) {
    FILE *fp = fopen(filename, "r");
    FILE *tmp_fp = fopen("temp.json", "w");
    char line[1024];
    int count = 0;

    if (fp == NULL || tmp_fp == NULL) {
        perror("Error opening file");
        return;
    }

    while (fgets(line, sizeof(line), fp)) {
        if (count == index) {
            fprintf(tmp_fp, "{ \"userId\": %d, \"id\": %d, \"title\": \"%s\", \"body\": \"%s\" }\n",
                    post->userId, post->id, post->title, post->body);
        } else {
            fputs(line, tmp_fp);
        }
        count++;
    }

    fclose(fp);
    fclose(tmp_fp);

    remove(filename);
    rename("temp.json", filename);
}

void deletePost(const char *filename, int index) {
    FILE *fp = fopen(filename, "r");
    FILE *tmp_fp = fopen("temp.json", "w");
    char line[1024];
    int count = 0;

    if (fp == NULL || tmp_fp == NULL) {
        perror("Error opening file");
        return;
    }

    while (fgets(line, sizeof(line), fp)) {
        if (count != index) {
            fputs(line, tmp_fp);
        }
        count++;
    }

    fclose(fp);
    fclose(tmp_fp);

    remove(filename);
    rename("temp.json", filename);
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_read;

    bytes_read = read(client_socket, buffer, BUFFER_SIZE - 1);
    if (bytes_read < 0) {
        perror("read");
        close(client_socket);
        return;
    }

    buffer[bytes_read] = '\0';
    printf("Request:\n%s\n", buffer);

    char method[8];
    char path[256];
    sscanf(buffer, "%s %s", method, path);

    if (strcmp(method, "GET") == 0 && strcmp(path, "/posts") == 0) {
        const char *response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n"
            "\r\n"
            "<!DOCTYPE html>"
            "<html>"
            "<head><title>Posts</title></head>"
            "<body><h1>Posts</h1><pre>";

        write(client_socket, response, strlen(response));
        readPosts(FILENAME);
        const char *response_end =
            "</pre></body>"
            "</html>";
        write(client_socket, response_end, strlen(response_end));

    } else if (strcmp(method, "POST") == 0 && strcmp(path, "/posts") == 0) {
        struct Post newPost = {1, 1, "New Title", "New body of the post"};
        createPost(FILENAME, &newPost);
        const char *response =
            "HTTP/1.1 201 Created\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n"
            "\r\n"
            "<!DOCTYPE html>"
            "<html>"
            "<head><title>Created</title></head>"
            "<body><h1>Post Created</h1></body>"
            "</html>";
        write(client_socket, response, strlen(response));

    } else if (strcmp(method, "PUT") == 0 && strncmp(path, "/posts/", 7) == 0) {
        int index = atoi(path + 7);
        struct Post updatedPost = {1, 1, "Updated Title", "Updated body of the post"};
        updatePost(FILENAME, index, &updatedPost);
        const char *response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n"
            "\r\n"
            "<!DOCTYPE html>"
            "<html>"
            "<head><title>Updated</title></head>"
            "<body><h1>Post Updated</h1></body>"
            "</html>";
        write(client_socket, response, strlen(response));

    } else if (strcmp(method, "DELETE") == 0 && strncmp(path, "/posts/", 7) == 0) {
        int index = atoi(path + 7);
        deletePost(FILENAME, index);
        const char *response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n"
            "\r\n"
            "<!DOCTYPE html>"
            "<html>"
            "<head><title>Deleted</title></head>"
            "<body><h1>Post Deleted</h1></body>"
            "</html>";
        write(client_socket, response, strlen(response));

    } else {
        const char *response =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n"
            "\r\n"
            "<!DOCTYPE html>"
            "<html>"
            "<head><title>Not Found</title></head>"
            "<body><h1>404 Not Found</h1></body>"
            "</html>";
        write(client_socket, response, strlen(response));
    }

    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 10) < 0) {
        perror("listen");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        client_addr_len = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            perror("accept");
            close(server_socket);
            exit(EXIT_FAILURE);
        }

        handle_client(client_socket);
    }

    close(server_socket);

    return 0;
}
