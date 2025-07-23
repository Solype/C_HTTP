#include "http_server.h"
#include "utils/http_log.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static int http_server_init(struct http_server_s *server, int port, int nb)
{
    int opt = 1;

    server->clean_quit = false;
    server->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server->socket < 0) {
        return log_error("Failed to create socket");
    }
    if (setsockopt(server->socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        log_warning("Failed to set socket options");
    }

    server->addr = (struct sockaddr_in) {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(port)
    };

    if (bind(server->socket, (struct sockaddr *)&server->addr, sizeof(server->addr)) < 0) {
        close(server->socket);
        return log_error("Failed to bind socket");;
    }
    if (listen(server->socket, nb) < 0) {
        close(server->socket);
        return log_error("Failed to listen on socket");;
    }
    return log_success("Server started at http://%s:%d", inet_ntoa(server->addr.sin_addr), port);
}

struct http_server_s *http_server_create(int port, int nb)
{
    struct http_server_s *server = malloc(sizeof(struct http_server_s));
    if (server == NULL) {
        return NULL;
    }
    if (http_server_init(server, port, nb) != 0) {
        free(server);
        return NULL;
    }
    return server;
}

int http_server_destroy(struct http_server_s *server)
{
    close(server->socket);
    free(server);
    log_info("Server stopped");
    return 0;
}

int http_server_enable_clean_quit(struct http_server_s *server)
{
    server->clean_quit = true;
    log_info("Clean quit enabled, press Ctrl+C to stop server");
    return 0;
}
