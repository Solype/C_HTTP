#include "utils/http_method.h"

#ifndef HEADER_H_
    #define HEADER_H_


struct string_pair {
    char *key;
    char *value;
};

/**
 * @brief The header structure
 */
struct header_s {
    enum method_e method;               // the method of the request
    char *uri;                          // the uri of the request
    char *version;                      // the version of the request
    struct string_pair *pairs;          // the pairs of the request, all the header values
    struct string_pair *query_params;   // the query parameters of the request, after the "?"
    int header_count;                   // the number of header pairs
    int query_param_count;              // the number of query parameters
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

/**
 * @brief Retrieve the value in the header of the key given
 * 
 * @param header The header structure to retrieve from the information
 * @param key the key of the wanted value
 * @return NULL in case of absence of the element, the pointer to the string in case of success
 */
char const *header_get(struct header_s const *header, char const *key);

/**
 * @brief Retrieve the value in the URI query parameters of the key given
 * 
 * @param header The header structure to retrieve from the information
 * @param key the key of the wanted value
 * @return NULL in case of absence of the element, the pointer to the string in case of success
 */
char const *header_query_get(struct header_s const *header, char const *key);

#endif /* !HEADER_H_ */
