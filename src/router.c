#include "http_log.h"
#include "router.h"
#include <stdlib.h>
#include <string.h>

static int route_init(struct __route_tree_s *route)
{
    (void)route;
    return 0;
}

static void swap_route(route_t *a, route_t *b)
{
    route_t tmp = *a;

    *a = *b;
    *b = tmp;
}

static void sort_routes(route_t routes[], size_t nb_routes)
{
    if (nb_routes == 0) {
        return;
    }

    for (size_t i = 0; i < nb_routes - 1; ++i) {
        for (size_t j = i + 1; j < nb_routes; ++j) {
            if (strcmp(routes[i].path, routes[j].path) > 1) {
                swap_route(&(routes[i]), &(routes[j]));
            }
        }
    }
}

router_t *router_init(route_t routes[], size_t nb_routes)
{
    struct __route_tree_s *root = malloc(sizeof(struct __route_tree_s));

    if (root == NULL) {
        return NULL;
    }

    root->child = NULL;
    root->default_child = NULL;
    root->handler = NULL;
    root->path = NULL;
    route_init(root);
    sort_routes(routes, nb_routes);
    for (size_t i = 0; i < nb_routes; ++i) {
        log_info("%s", routes[i].path);
    }
    return NULL;
}

void router_destroy(router_t *tree)
{
    free(tree);
}


