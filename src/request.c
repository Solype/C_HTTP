#include "request.h"
#include "header.h"
#include "utils/http_log.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

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

#define BUF_SIZE 256

int request_init(struct request_s *request, int client_socket)
{
    char buffer[BUF_SIZE];
    ssize_t total_read = 0;
    ssize_t bytes_read;
    char *end_of_headers;
    int header_done = 0;

    request->raw_request = malloc(BUF_SIZE);
    if (!request->raw_request) return log_error("Memory allocation failed");

    while ((bytes_read = read(client_socket, buffer, sizeof(buffer))) > 0) {
        if (total_read + bytes_read >= BUF_SIZE) {
            request->raw_request = realloc(request->raw_request, total_read + bytes_read + 1);
            if (!request->raw_request) return log_error("Reallocation failed");
        }

        memcpy(request->raw_request + total_read, buffer, bytes_read);
        total_read += bytes_read;
        request->raw_request[total_read] = '\0';

        if (!header_done && (end_of_headers = strstr(request->raw_request, "\r\n\r\n"))) {
            header_done = 1;
            break; // Stop here if you want only headers first
        }
    }

    if (bytes_read <= 0) {
        free(request->raw_request);
        return log_error("Failed to read from client");
    }

    log_info("Allocated %d bytes", total_read);

    char *body;
    request_get_ip(request, client_socket);
    header_init(&request->headers, request->raw_request, &body);
    request->body = body;

    return 0;
}


int request_destroy(struct request_s *request)
{
    free(request->raw_request);
    header_destroy(&request->headers);
    return 0;
}
