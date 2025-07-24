#define _GNU_SOURCE
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include "http_server.h"
#include "utils/http_log.h"
#include "request.h"
#include "router/router.h"
#include "response_brut_value.h"


static bool_t sig_received = false;

static void handle_sig(int sig __attribute__((unused)))
{
    fprintf(stderr, "\nsignal %d (%s) received\n", sig, strsignal(sig));
    sig_received = true;
}

static void handle_response(int client_socket, struct response_s *response)
{
    const char *status_message = (response->status_message == NULL &&
        response->status_code < NB_STATUS_CODES) ? "OK" : response->status_message;
    size_t body_len = response->body != NULL ? strlen(response->body) : 0;

    if (response->content_type > NB_CONTENT_TYPE) {
        response->content_type = no_body;
    }
    if (response->body == NULL || response->content_type == no_body || body_len == 0) {
        dprintf(client_socket, basic_response_message_no_body, response->status_code, status_message);
    } else {
        dprintf(client_socket, basic_response_message,
            response->status_code,
            status_message,
            content_types_str[response->content_type],
            body_len,
            response->body);
    }
    if (body_len >= 1024 && response->auto_free) {
        free(response->body);
    }
}

static void format_automatic_404(struct response_s *response)
{
    static char const body[] = "<html><body><h1>404 Not Found</h1></body></html>";

    response->status_code = 404;
    response->status_message = "Not Found";
    memcpy(response->body, body, sizeof(body));
    response->content_type = text_html;
    response->auto_free = true;
}

static void format_automatic_500(struct response_s *response)
{
    static char const body[] = "<html><body><h1>500 Internal Server Error</h1></body></html>";

    response->status_code = 500;
    response->status_message = "Internal Server Error";
    memcpy(response->body, body, sizeof(body));
    response->content_type = text_html;
    response->auto_free = true;
}

static void handle_client(int client_socket, router_t router, struct handler_env_s *env)
{
    struct request_s request;
    handler_t handler;
    struct response_s response;
    char buffer[1024] = {0};

    response.body = buffer;
    response.auto_free = true;
    response.content_type = no_body;
    response.status_code = 200;
    response.status_message = NULL;
    if (request_init(&request, client_socket) != EXIT_SUCCESS) {
        close(client_socket);
        return;
    }
    handler = router_get_handler(router, request.headers.uri, request.headers.method, env);
    if (handler != NULL) {
        if (handler(&request, env, &response) != EXIT_SUCCESS) {
            log_error("Failed to handle request");
            format_automatic_500(&response);
        } else {
            log_success("Request handled");
        }
    } else {
        format_automatic_404(&response);
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
    struct timeval timeout = {.tv_sec = 2, .tv_usec = 0};
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
