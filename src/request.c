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

#define BUF_SIZE 128

static int read_header(struct request_s *request, int client_fd, char **end_of_header)
{
    ssize_t bytes_read;
    char buffer[BUF_SIZE];
    int total_read = 0;

    while ((bytes_read = read(client_fd, buffer, sizeof(buffer))) > 0) {
        if (total_read + bytes_read >= BUF_SIZE) {
            request->raw_request = realloc(request->raw_request, total_read + bytes_read + 1);
            if (!request->raw_request) return log_error("Reallocation failed");
        }
        memcpy(request->raw_request + total_read, buffer, bytes_read);
        request->raw_request[total_read + bytes_read] = '\0';
        *end_of_header = (total_read < 4) ? NULL : strstr(request->raw_request + total_read - 4, "\r\n\r\n");
        if (total_read > 4 && *end_of_header != NULL) {
            total_read += bytes_read;
            return total_read;
        }
        total_read += bytes_read;
    }
    return total_read;
}

static int read_body(struct request_s *request, int client_fd,
                     char const *end_of_header, int total_read)
{
    char const *content_len_str = header_get(&request->headers, "Content-Length");
    size_t content_len;
    const char *body_start = end_of_header + 3; // CRLF (\r\n\r\n) → 4 bytes
    size_t header_size = body_start - request->raw_request;
    size_t already_read = total_read - header_size;
    size_t total_to_read;
    ssize_t r;

    request->body = NULL;
    if (content_len_str == NULL) {
        return 0;
    }
    content_len = atoi(content_len_str);
    if (content_len == 0) {
        return 0;
    }
    total_to_read = content_len - already_read;
    request->body = malloc(content_len + 1);
    if (!request->body) {
        return log_error("malloc failed");
    }
    already_read = already_read > content_len ? content_len : already_read;
    memcpy(request->body, body_start, already_read);
    request->body[already_read] = '\0';
    if (total_to_read == 0) {
        return 0;
    }
    memset(request->body + already_read, 0, total_to_read);
    r = read(client_fd, request->body + already_read, total_to_read);
    request->body[content_len] = '\0';
    return r;
}

int request_init(struct request_s *request, int client_socket)
{
    ssize_t total_read = 0;
    char *end_of_header = NULL;

    request_get_ip(request, client_socket);
    request->raw_request = malloc(BUF_SIZE);
    if (!request->raw_request) {
        return log_error("Memory allocation failed");
    }
    total_read = read_header(request, client_socket, &end_of_header);
    if (total_read <= 0) {
        free(request->raw_request);
        return log_error("Failed to read from client");
    }
    header_init(&request->headers, request->raw_request, &end_of_header);
    read_body(request, client_socket, end_of_header, total_read);
    return 0;
}


int request_destroy(struct request_s *request)
{
    free(request->raw_request);
    free(request->body);
    header_destroy(&request->headers);
    return 0;
}
