#ifndef HTTP_METHOD_H_
    #define HTTP_METHOD_H_

enum method_e {
    GET = 0,
    POST = 1,
    PUT = 2,
    DELETE = 3,
    HEAD = 4,
    OPTIONS = 5,
    TRACE = 6,
    NB_METHODS = 7,
    ERROR = -1
} ;

struct method_pair_s {
    char *literal;
    enum method_e method;
};

/**
 * @brief Get the method enum from a string
 * 
 * @param method The string to get the method from
 * @return method_e, see method_e enum
 */
enum method_e get_method(char const *method);

#endif /* !HTTP_METHOD_H_ */

