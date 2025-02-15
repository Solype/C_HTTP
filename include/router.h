#include "request.h"
#include "http_method.h"

#ifndef ROUTE_TREE_H_
    #define ROUTE_TREE_H_

typedef int (*handler_t)(struct request_s *request);

struct __route_tree_s {
    char *path;
    struct __route_tree_s *child[50];
    struct __route_tree_s *default_child;
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
