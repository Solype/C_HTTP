#include "http_server.h"
#include "http_log.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


int http_server_init(struct http_server_s *server, int port, int nb)
{
    server->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server->socket < 0) {
        return log_error("Failed to create socket");
    }
    log_success("Socket created");

    server->addr = (struct sockaddr_in) {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(port)
    };

    if (bind(server->socket, (struct sockaddr *)&server->addr, sizeof(server->addr)) < 0) {
        close(server->socket);
        return log_error("Failed to bind socket");;
    }
    log_success("Socket binded");
    if (listen(server->socket, nb) < 0) {
        close(server->socket);
        return log_error("Failed to listen on socket");;
    }
    return log_success("Server started");
}

int http_server_destroy(struct http_server_s *server)
{
    close(server->socket);
    log_info("Server stopped");
    return 0;
}

int http_server_enable_clean_quit(struct http_server_s *server)
{
    server->clean_quit = true;
    log_info("Clean quit enabled, press Ctrl+C to stop server");
    return 0;
}
