#define _GNU_SOURCE
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include "http_server.h"
#include "http_log.h"
#include "request.h"
#include "router.h"
#include "response_brut_value.h"


static bool_t sig_received = false;

static void handle_sig(int sig __attribute__((unused)))
{
    fprintf(stderr, "\nsignal %d (%s) received\n", sig, strsignal(sig));
    sig_received = true;
}

static void handle_response(int client_socket, struct response_s *response)
{
    static const char *message = "HTTP/1.1 %.3d %s\r\n"
                                 "Content-Type: %s\r\n"
                                 "Content-Length: %u\r\n"
                                 "Connection: close\r\n\r\n%s";
    const char *status_message = (response->status_message == NULL &&
        response->status_code < NB_STATUS_CODES) ? "OK" : response->status_message;

    if (response->body == NULL) {
        response->body = "";
    }
    if (response->content_type > NB_CONTENT_TYPE) {
        response->content_type = application_octet_stream;
    }
    dprintf(client_socket, message,
        response->status_code,
        status_message,
        content_types_str[response->content_type],
        strlen(response->body),
        response->body);
}

static void handle_client(int client_socket, router_t router, struct handler_env_s *env)
{
    struct request_s request;
    handler_t handler;
    struct response_s response;

    response.body = NULL;
    response.content_type = 0;
    response.status_code = 200;
    response.status_message = NULL;
    if (request_init(&request, client_socket) != EXIT_SUCCESS) {
        return;
    }
    handler = router_get_handler(router, request.headers.uri, request.headers.method, env);
    if (handler != NULL) {
        if (handler(&request, env, &response) != EXIT_SUCCESS) {
            log_error("Failed to handle request");
        } else {
            log_success("Request handled");
        }
    }
    request_destroy(&request);
    handle_response(client_socket, &response);
    close(client_socket);
}

static int init_select(fd_set *readfds, int socket)
{
    FD_ZERO(readfds);
    FD_SET(socket, readfds);
    FD_SET(STDIN_FILENO, readfds);
    return EXIT_SUCCESS;
}

static int get_client(int socket, fd_set *readfds)
{
    static struct timeval timeout = {.tv_sec = 0, .tv_usec = 0};
    int ret = select(socket + 1, readfds, NULL, NULL, &timeout);
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    if (ret == -1) {
        return -log_error("Failed to select");
    }
    if (!FD_ISSET(socket, readfds)) {
        return -1;
    }
    return accept(socket, (struct sockaddr *)&client_addr, &client_len);
}

int http_server_run(struct http_server_s *server, router_t router)
{
    struct handler_env_s env;
    int client_socket;
    fd_set readfds;
    fd_set copy_readfds;

    if (init_select(&readfds, server->socket) != EXIT_SUCCESS) {
        return log_error("Failed to initialize select");
    }
    handler_env_init(&env);
    if (server->clean_quit == true) {
        signal(SIGINT, handle_sig);
    } else {
        log_warning("Clean quit not enabled");
    }
    while (sig_received == false) {
        copy_readfds = readfds;
        client_socket = get_client(server->socket, &copy_readfds);
        if (client_socket == -1) {
            continue;
        }
        handle_client(client_socket, router, &env);
    }
    handler_env_destroy(&env);
    return log_success("Server while loop just stopped");
}
