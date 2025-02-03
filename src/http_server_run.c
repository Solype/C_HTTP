#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "http_server.h"
#include "http_log.h"
#include "request.h"


static bool http_server_running = true;

static void handle_sig(int sig __attribute__((unused)))
{
    fprintf(stderr, "\n");
    http_server_running = false;
}

static void handle_client(int client_socket)
{
    struct request_s request;
    const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 11\r\n\r\nbonjour";

    if (request_init(&request, client_socket) != EXIT_SUCCESS) {
        return;
    }
    request_destroy(&request);
    if (write(client_socket, response, strlen(response)) == -1) {
        log_error("Failed to write to client");
    }
    close(client_socket);
}

int http_server_run(struct http_server_s *server)
{
    if (server->clean_quit == true) {
        signal(SIGINT, handle_sig);
    } else {
        log_warning("Clean quit not enabled");
    }
    while (http_server_running == true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server->socket, (struct sockaddr *)&client_addr, &client_len);
        if (http_server_running == false) {
            break;
        }
        if (client_socket == -1) {
            return log_error("Failed to accept client");
        }
        handle_client(client_socket);
    }
    return log_success("Server while loop just stopped");
}
