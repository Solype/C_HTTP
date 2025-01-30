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
    char buffer[1024] = {0}; // Initialise le buffer à 0
    ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);

    if (bytes_read <= 0) {
        log_error("Failed to read from client");
        close(client_socket);
        return;
    }

    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    if (getpeername(client_socket, (struct sockaddr*)&client_addr, &addr_len) == 0) {
        char client_ip[INET_ADDRSTRLEN]; // Stocke l'IP en format lisible
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        log_info("Client connected : %s:%d", client_ip, ntohs(client_addr.sin_port));
    } else {
        log_error("getpeername failed");
    }



    request_init(&request, buffer);


    const char *response = "HTTP/1.1 400 OK\r\n";
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
