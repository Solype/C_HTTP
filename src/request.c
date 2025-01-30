#include "request.h"
#include "header.h"
#include "http_log.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

static int request_get_ip(struct request_s *request, int client_socket) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    if (getpeername(client_socket, (struct sockaddr*)&client_addr, &addr_len) == 0) {
        inet_ntop(AF_INET, &client_addr.sin_addr, request->ip, INET_ADDRSTRLEN);
        return log_info("Client connected : %s:%d", request->ip, ntohs(client_addr.sin_port));
    } else {
        return log_warning("getpeername failed");
    }
}

int request_init(struct request_s *request, int client_socket)
{
    char *body;
    request->raw_request = calloc(1024, sizeof(char));
    ssize_t bytes_read = read(client_socket, request->raw_request, 1024);

    if (bytes_read <= 0) {
        return log_error("Failed to read from client");;
    }
    request_get_ip(request, client_socket);
    header_init(&request->headers, request->raw_request, &body);
    request->body = body;
    log_info("Body received: %s", request->body);
    return 0;
}

int request_destroy(struct request_s *request)
{
    free(request->raw_request);
    header_destroy(&request->headers);
    return 0;
}
