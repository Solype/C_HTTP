#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "http_server.h"
#include "http_log.h"
#include "request.h"
#include "router.h"


static bool_t sig_received = true;

static void handle_sig(int sig __attribute__((unused)))
{
    fprintf(stderr, "\n");
    sig_received = false;
}

static void handle_client(int client_socket, router_t router, struct handler_env_s *env)
{
    struct request_s request;
    handler_t handler;
    const char *response =
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 9\r\n"
        "Connection: close\r\n\r\n"
        "Not Found";


    if (request_init(&request, client_socket) != EXIT_SUCCESS) {
        return;
    }
    handler = router_get_handler(router, request.headers.uri, request.headers.method, env);
    if (handler != NULL) {
        if (handler(&request) != EXIT_SUCCESS) {
            log_error("Failed to handle request");
        } else {
            log_success("Request handled");
        }
    }
    request_destroy(&request);
    if (write(client_socket, response, strlen(response)) == -1) {
        log_error("Failed to write to client");
    }
    close(client_socket);
}

int http_server_run(struct http_server_s *server, router_t router)
{
    struct handler_env_s env;

    handler_env_init(&env);
    if (server->clean_quit == true) {
        signal(SIGINT, handle_sig);
    } else {
        log_warning("Clean quit not enabled");
    }
    while (sig_received == true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server->socket, (struct sockaddr *)&client_addr, &client_len);
        if (sig_received == false) {
            break;
        }
        if (client_socket == -1) {
            return log_error("Failed to accept client");
        }
        handle_client(client_socket, router, &env);
    }
    handler_env_destroy(&env);
    return log_success("Server while loop just stopped");
}
