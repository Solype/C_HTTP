#include "utils/http_log.h"
#include "header.h"
#include "utils/http_method.h"

#include <stdlib.h>
#include <string.h>

static int count_header_lines(char *raw_request)
{
    int count = 0;

    while (*raw_request != '\0' && *raw_request != '\n') {
        if (*raw_request == '\r' && *(raw_request + 1) == '\n') {
            count++;
            raw_request += 2;
            continue;
        }
        ++raw_request;
    }
    return count;
}

static int init_pairs(struct header_s *header, char *raw_request, char **body)
{
    char *save_pointer;

    header->header_count = count_header_lines(raw_request) - 2;
    header->pairs = calloc(header->header_count, sizeof(struct string_pair));
    if (header->pairs == NULL) {
        return log_error("Failed to allocate memory");
    }
    raw_request = strchr(raw_request, '\n') + 1;

    for (int i = 0; i < header->header_count; i++) {
        header->pairs[i].key = __strtok_r(raw_request, ": ", &save_pointer);
        if (header->pairs[i].key == NULL) {
            return log_error("Invalid key header");
        }
        if (header->pairs[i].key[0] == '\n') {
            header->pairs[i].key++;
        }
        raw_request = save_pointer;
        header->pairs[i].value = __strtok_r(raw_request, "\r\n", &save_pointer);
        if (header->pairs[i].value == NULL) {
            return log_error("Invalid value header");
        }
        raw_request = save_pointer;
    }
    *body = raw_request;
    return EXIT_SUCCESS;
}

static int query_param_count(char const *query)
{
    int count = 0;

    while (*query != '\0') {
        if (*query == '&') {
            count++;
        }
        query++;
    }
    return count;
}

static int init_query(struct header_s *header)
{
    char *query = strchr(header->uri, '?');
    char *save_pointer;

    if (query == NULL) {
        return log_info("No query parameter on url: %s", header->uri);;
    }
    *query = '\0';
    query++;
    header->query_param_count = query_param_count(query) + 1;
    header->query_params = malloc(sizeof(struct string_pair[header->query_param_count]));
    if (header->query_params == NULL) {
        return log_error("Failed to allocate memory");
    }
    for (int i = 0; i < header->query_param_count; i++) {
        header->query_params[i].key = __strtok_r(query, "=", &save_pointer);
        if (header->query_params[i].key == NULL) {
            return log_error("Invalid key query");
        }
        if (header->query_params[i].key[0] == '\n') {
            header->query_params[i].key++;
        }
        query = save_pointer;
        header->query_params[i].value = __strtok_r(query, "&", &save_pointer);
        if (header->query_params[i].value == NULL) {
            return log_error("Invalid value query");
        }
        query = save_pointer;
    }
    return EXIT_SUCCESS;
}

int header_init(struct header_s *header, char *raw_request, char **body)
{
    char *method;

    if (init_pairs(header, raw_request, body) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }
    method = __strtok_r(raw_request, " ", &(header->uri));
    header->method = get_method(method);
    __strtok_r(header->uri, " \r\n", &(header->version));
    if (header->uri == NULL || header->version == NULL || header->method == ERROR) {
        return log_error("Invalid request: %03b", (header->uri == NULL) << 0 | (header->version == NULL) << 1 | (header->method == ERROR) << 2);
    }
    init_query(header);
    return log_success("Request %s with method %s", header->uri, method, header->version);
}

int header_destroy(struct header_s *header)
{
    free(header->pairs);
    return 0;
}
