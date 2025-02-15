#include "http_log.h"
#include "router.h"
#include "http_method.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

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

static void shorten_path(route_t *routes)
{
    size_t len;

    len = strlen(routes->path);
    while (len > 1 && routes->path[len - 1] == '/') {
        routes->path[--len] = '\0';
    }
}

static size_t remove_route_uninitializable(route_t routes[], size_t nb_routes)
{
    int error = 0;

    for (size_t i = 0; i < nb_routes; ++i) {
        error = 0;
        if (routes[i].path == NULL || routes[i].path[0] != '/' || strchr(routes[i].path, ' ') != NULL)
            error = 1 + log_warning("Route has invalid path and will be ignored");
        // if (routes[i].handler == NULL)
        //     error = 1 + log_warning("Route has no handler and will be ignored");
        if (routes[i].method == NULL || test_route_method_validity(&routes[i]) != EXIT_SUCCESS)
            error = 1 + log_warning("Route has no method or invalid method and will be ignored");
        if (error == 1) {
            routes[i] = routes[nb_routes - 1];
            --i;
            --nb_routes;
        } else {
            shorten_path(&(routes[i]));
        }
    }
    return nb_routes;
}

static int hash(char const *path, int size)
{
    int hash = 0;

    for (int i = 0; path[i] != '\0' && i < size; ++i) {
        hash += path[i] + i;
        hash = hash % 50;
    }
    return hash;
}

static int insert_route(struct __route_tree_s *tree, route_t *route)
{
    (void)tree;
    char *save_pointer = route->path;
    char *slash_addr = strchr(route->path + 1, '/');
    int index;
    
    if (slash_addr == NULL) {
        index = hash(route->path, strlen(route->path));
        log_warning("found a route, index %d", index);
    } else {
        route->path = slash_addr;
        index = ((intptr_t)save_pointer > (intptr_t)route->path) ? save_pointer - route->path : route->path - save_pointer;
        index = hash(save_pointer, index);
        log_info("recursion");
        insert_route(tree, route);
    }

    return 0;
}


router_t *router_init(route_t routes[], size_t nb_routes)
{
    struct __route_tree_s *root = malloc(sizeof(struct __route_tree_s));

    if (root == NULL) {
        return NULL;
    }
    log_info("Checking routes validity");
    nb_routes = remove_route_uninitializable(routes, nb_routes);
    root->default_child = NULL;
    memset(root->handler, 0, sizeof(root->handler));
    memset(root->child, 0, sizeof(root->child));
    root->path = "/";
    for (size_t i = 0; i < nb_routes; ++i) {
        log_info("Inserting route %s", routes[i].path);
        if (routes[i].path[1] == '\0') {
            log_info("found the root, root");
        } else {
            insert_route(root, &(routes[i]));
        }
    }
    return (router_t *)root;
}

void router_destroy(router_t *tree)
{
    free(tree);
}


