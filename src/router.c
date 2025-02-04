#include "router.h"
#include <stdlib.h>



router_t *route_tree_init(route_t routes[])
{
    struct __route_tree_s *root = malloc(sizeof(struct __route_tree_s));

    if (root == NULL) {
        return NULL;
    }
    root->child = NULL;
    root->default_child = NULL;
    root->handler = NULL;
    root->path = NULL;
    (void)routes;
    return NULL;
}

void route_tree_destroy(router_t *tree)
{
    (void)tree;
}


