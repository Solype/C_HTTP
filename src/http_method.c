#include "http_method.h"
#include "http_log.h"

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

enum method_e get_method(char const *method)
{
    for (unsigned int i = 0; i < sizeof(method_pairs) / sizeof(method_pairs[0]); i++) {
        if (strcmp(method, method_pairs[i].literal) == 0) {
            return method_pairs[i].method;
        }
    }
    log_error("Unknown method: %s", method);
    return ERROR;
}

char const * const get_method_name(enum method_e method) {
    switch (method) {
        case GET:
            return "GET";
        case POST:
            return "POST";
        case PUT:
            return "PUT";
        case DELETE:
            return "DELETE";
        case HEAD:
            return "HEAD";
        case OPTIONS:
            return "OPTIONS";
        case TRACE:
            return "TRACE";
        default:
            return "UNKNOWN";
    }
}
