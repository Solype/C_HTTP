#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "http_server.h"
#include "http_log.h"
#include "request.h"


static bool http_server_running = true;

static void handle_sig(int sig __attribute__((unused)))
{
    http_server_running = false;
}

static void handle_client(int client_socket)
{
    struct request_s request;
    const char *response = "HTTP/1.1 400 OK\r\n";

    request_init(&request, client_socket);

    if (write(client_socket, response, strlen(response)) == -1) {
        log_error("Failed to write to client");
    }
    close(client_socket);
}

int http_server_run(struct http_server_s *server)
{
    if (server->clean_quit == true) {
        signal(SIGINT, handle_sig);
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
