#include "http_log.h"
#include "router.h"
#include "http_method.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static int route_init(struct __route_tree_s *route)
{
    (void)route;
    return 0;
}

static void swap_route(route_t *a, route_t *b)
{
    a->path = (char *)((uintptr_t)b->path ^ (uintptr_t)a->path);
    b->path = (char *)((uintptr_t)b->path ^ (uintptr_t)a->path);
    a->path = (char *)((uintptr_t)b->path ^ (uintptr_t)a->path);

    a->method = (char *)((uintptr_t)b->method ^ (uintptr_t)a->method);
    b->method = (char *)((uintptr_t)b->method ^ (uintptr_t)a->method);
    a->method = (char *)((uintptr_t)b->method ^ (uintptr_t)a->method);

    a->handler = (handler_t)((uintptr_t)b->handler ^ (uintptr_t)a->handler);
    b->handler = (handler_t)((uintptr_t)b->handler ^ (uintptr_t)a->handler);
    a->handler = (handler_t)((uintptr_t)b->handler ^ (uintptr_t)a->handler);
}

static void sort_routes(route_t routes[], size_t nb_routes)
{
    if (nb_routes == 0) {
        return;
    }

    for (size_t i = 0; i < nb_routes - 1; ++i) {
        for (size_t j = i + 1; j < nb_routes; ++j) {
            if (strcmp(routes[i].path, routes[j].path) > 0) {
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

static size_t remove_route_uninitializable(route_t routes[], size_t nb_routes)
{
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
            log_info("Removing route %s, replacing it with : %s", routes[i].path, routes[nb_routes - 1].path);
            routes[i] = routes[nb_routes - 1];
            --i;
            --nb_routes;
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
    nb_routes = remove_route_uninitializable(routes, nb_routes);
    root->child = NULL;
    root->default_child = NULL;
    memset(root->handler, 0, sizeof(root->handler));
    root->path = NULL;
    log_info("Sorting routes");
    sort_routes(routes, nb_routes);
    route_init(root);
    for (size_t i = 0; i < nb_routes; ++i) {
        log_info("Adding route %s", routes[i].path);
    }
    return (router_t *)root;
}

void router_destroy(router_t *tree)
{
    free(tree);
}


