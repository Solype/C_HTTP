

#ifndef HEADER_H_
    #define HEADER_H_

typedef enum {
    GET = 0,
    ERROR = 1,
    POST = 2,
    PUT = 3,
    DELETE = 4,
    HEAD = 5,
    OPTIONS = 6,
    TRACE = 7
} method_e;

struct method_pair_s {
    char *literal;
    method_e method;
};

struct string_pair {
    char *key;
    char *value;
};

struct header_s {
    method_e method;
    char *uri;
    char *version;
    struct string_pair *pairs;
    int count;
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
