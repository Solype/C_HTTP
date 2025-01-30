#include "http_server.h"
#include "request.h"
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

static bool http_server_running = true;

static void handle_sig(int sig __attribute__((unused)))
{
    http_server_running = false;
}

static void handle_client(int client_socket)
{
    struct request_s request;
    char buffer[1024] = {0}; // Initialise le buffer à 0
    ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);

    if (bytes_read <= 0) {
        perror("Failed to read from client");
        close(client_socket);
        return;
    }

    request_init(&request, buffer);
    request_display(&request);
    const char *response =
        "HTTP/1.1 400 OK\r\n";

    if (write(client_socket, response, strlen(response)) == -1) {
        perror("Failed to write to client");
    }
    close(client_socket);
}

int http_server_init(struct http_server_s *server, int port, int nb)
{
    server->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server->socket == -1) {
        perror("Failed to create socket");
        return EXIT_FAILURE;
    }

    server->addr = (struct sockaddr_in) {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(port)
    };

    if (bind(server->socket, (struct sockaddr *)&server->addr, sizeof(server->addr)) < 0) {
        perror("Failed to bind socket");
        close(server->socket);
        return EXIT_FAILURE;
    }
    if (listen(server->socket, nb) < 0) {
        perror("Failed to listen on socket");
        close(server->socket);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int http_server_destroy(struct http_server_s *server)
{
    close(server->socket);
    return 0;
}

int http_server_run(struct http_server_s *server)
{
    signal(SIGINT, handle_sig);
    while (http_server_running == true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server->socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket == -1) {
            perror("Accept failed");
            continue;
        }
        handle_client(client_socket);
    }
    return 0;
}

int http_server_enable_clean_quit(struct http_server_s *server __attribute__((unused)))
{
    return 0;
}
