#include "header.h"

#ifndef REQUEST_H_
    #define REQUEST_H_


struct request_s {
    char *raw_request;
    char *uri;
    char *method;
    char *version;
    char *rest;
    struct header_s headers;
};

/**
 * @brief Initialize the request structure
 * 
 * @param request The request structure to initialize
 * @param raw_request The raw request to parse
 */
int request_init(struct request_s *request, char *raw_request);

/**
 * @brief Destroy the request structure
 * 
 * @param request The request structure to destroy
 * @return 0 on success, -1 on failure
 */
int request_destroy(struct request_s *request);


#endif /* !REQUEST_H_ */
