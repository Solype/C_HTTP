#include "http_log.h"
#include "router.h"
#include "http_method.h"
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

static int test_route_method_validity(route_t *routes)
{
    if (routes->method == NULL || routes->method[0] == '\0') {
        return EXIT_FAILURE;
    }
    if (get_method(routes->method) == ERROR) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

static size_t check_routes_validity(route_t routes[], size_t nb_routes)
{
    size_t removed_routes = 0;
    int error = 0;

    for (size_t i = 0; i < nb_routes; ++i) {
        error = 0;
        if (routes[i].path == NULL || routes[i].path[0] != '/')
            error = 1 + log_warning("Route has invalid path and will be ignored");
        // if (routes[i].handler == NULL)
        //     error = 1 + log_warning("Route has no handler and will be ignored");
        if (routes[i].method == NULL || test_route_method_validity(&routes[i]) != EXIT_SUCCESS)
            error = 1 + log_warning("Route has no method or invalid method and will be ignored");
        if (error == 1) {
            ++removed_routes;
            routes[i] = routes[nb_routes - 1];
            --i;
            nb_routes -= 1;
        }
    }
    return nb_routes;
}

router_t *router_init(route_t routes[], size_t nb_routes)
{
    struct __route_tree_s *root = malloc(sizeof(struct __route_tree_s));

    if (root == NULL) {
        return NULL;
    }
    log_info("Checking routes validity");
    nb_routes = check_routes_validity(routes, nb_routes);
    root->child = NULL;
    root->default_child = NULL;
    root->handler = NULL;
    root->path = NULL;
    log_info("Sorting routes");
    sort_routes(routes, nb_routes);

    route_init(root);
    for (size_t i = 0; i < nb_routes; ++i) {
        log_info("Adding route %s", routes[i].path);
    }
    return NULL;
}

void router_destroy(router_t *tree)
{
    free(tree);
}


