#include "request.h"
#include "header.h"
#include "http_log.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
    // request->raw_request = raw_request;
    // request->method = strtok(request->raw_request, " ");
    // request->uri = strtok(NULL, " ");
    // request->version = strtok(NULL, "\r\n");
    // request->rest = strtok(NULL, "");

    request_get_ip(request, client_socket);
    // header_init(&request->headers);
    return 0;
}

int request_destroy(struct request_s *request)
{
    free(request->raw_request);
    return 0;
}
