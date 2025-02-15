#ifndef HTTP_METHOD_H_
    #define HTTP_METHOD_H_

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

/**
 * @brief Get the method enum from a string
 * 
 * @param method The string to get the method from
 * @return method_e
 */
method_e get_method(char const *method);

#endif /* !HTTP_METHOD_H_ */

