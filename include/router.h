#include "request.h"
#include "http_method.h"

#ifndef ROUTE_TREE_H_
    #define ROUTE_TREE_H_

    #define HTTP_ROUTE_CHILD_COUNT 50

typedef int (*handler_t)(struct request_s *request);

struct __route_tree_s {
    char *path;
    size_t path_len;
    struct __route_tree_s *child[HTTP_ROUTE_CHILD_COUNT];
    struct __route_tree_s *default_child;
    size_t childs_count[HTTP_ROUTE_CHILD_COUNT];
    size_t default_childs_count;
    handler_t handler[NB_METHODS];
};


typedef struct route_s {
    char *path;
    char *method;
    handler_t handler;
} route_t;


typedef void *router_t;

router_t *router_init(route_t routes[], size_t nb_routes);

void router_destroy(router_t *tree);

#endif /* !ROUTE_TREE_H_ */
