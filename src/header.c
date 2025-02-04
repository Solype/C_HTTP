#include "http_log.h"
#include "header.h"

#include <stdlib.h>
#include <string.h>

static const struct method_pair_s method_pairs[] = {
    {"GET", GET},
    {"POST", POST},
    {"PUT", PUT},
    {"DELETE", DELETE},
    {"HEAD", HEAD},
    {"OPTIONS", OPTIONS},
    {"TRACE", TRACE}
};

static method_e get_method(char *method)
{
    for (unsigned int i = 0; i < sizeof(method_pairs) / sizeof(method_pairs[0]); i++) {
        if (strcmp(method, method_pairs[i].literal) == 0) {
            return method_pairs[i].method;
        }
    }
    return log_error("Unknown method");
}

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

    header->count = count_header_lines(raw_request) - 2;
    header->pairs = calloc(header->count, sizeof(struct string_pair));
    if (header->pairs == NULL) {
        return log_error("Failed to allocate memory");
    }
    raw_request = strchr(raw_request, '\n') + 1;

    for (int i = 0; i < header->count; i++) {
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

static void init_query(struct header_s *header)
{
    char *query = strchr(header->uri, '?');
    // char *save_pointer;
    int query_count = 0;

    if (query == NULL) {
        log_warning("No query on %s", header->uri);
        return;
    }
    // *query = '\0';
    // query++;
    query_count = query_param_count(query) + 1;

    log_info("Number of query infos: %d", query_count);
    // while (query != NULL && *query != '\0') {
        
    // }
}

int header_init(struct header_s *header, char *raw_request, char **body)
{
    char *method;

    if (init_pairs(header, raw_request, body) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }
    log_info("%s", raw_request);
    method = __strtok_r(raw_request, " ", &(header->uri));
    header->method = get_method(method);
    __strtok_r(header->uri, " \r\n", &(header->version));
    if (header->uri == NULL || header->version == NULL || header->method == ERROR) {
        return log_error("Invalid request: %03b", (header->uri == NULL) << 0 | (header->version == NULL) << 1 | (header->method == ERROR) << 2);
    }
    init_query(header);
    return log_success("Request %s with method %s and with http version : %s", header->uri, method, header->version);
}

int header_destroy(struct header_s *header)
{
    free(header->pairs);
    return 0;
}
