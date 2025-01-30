#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "http_server.h"

#define PORT 8080
#define BUFFER_SIZE 1024

static void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE] = {0}; // Initialise le buffer à 0
    ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
        perror("Failed to read from client");
        close(client_socket);
        return;
    }

    printf("Request:\n%s\n", buffer);

    const char *response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 11\r\n"
        "\r\n"
        "Hello world";

    if (write(client_socket, response, strlen(response)) == -1) {
        perror("Failed to write to client");
    }
    close(client_socket);
}

int main() {
    struct http_server_s server;
    http_server_init(&server, PORT, 5);
    http_server_enable_clean_quit(&server);
    http_server_run(&server);
    http_server_destroy(&server);
    return 0;
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(PORT)
    };

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) == -1) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server is running on port %d\n", PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket == -1) {
            perror("Accept failed");
            continue;
        }
        handle_client(client_socket);
    }

    close(server_socket);
    return 0;
}
