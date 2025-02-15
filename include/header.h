#include "http_method.h"

#ifndef HEADER_H_
    #define HEADER_H_


struct string_pair {
    char *key;
    char *value;
};

struct header_s {
    enum method_e method;
    char *uri;
    char *version;
    struct string_pair *pairs;
    struct string_pair *query_params;
    int header_count;
    int query_param_count;
};


/**
 * @brief Initialize the header structure
 *
 * @param header The header structure to initialize
 * @param raw_request The raw request to parse
 * @param body The body of the request
 * @return 0 on success, -1 on failure
 */
int header_init(struct header_s *header, char *raw_request, char **body);

/**
 * @brief Destroy the header structure
 *
 * @param header The header structure to destroy
 * @return 0 on success, -1 on failure
 */
int header_destroy(struct header_s *header);

#endif /* !HEADER_H_ */
