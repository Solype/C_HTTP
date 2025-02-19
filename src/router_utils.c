#include "http_log.h"
#include "router.h"
#include "router_utils.h"
#include "http_method.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static void display_router(struct __route_tree_s *tree, size_t depth)
{

    if (tree->path != NULL && tree->path_len > 0 && tree->path[0] != '\0') {
        log_info("%*s%.*s", depth * 2, "", (int)tree->path_len, tree->path);
    }
    for (size_t i = 0; i < HTTP_ROUTE_CHILD_COUNT; ++i) {
        for (size_t j = 0; j < tree->childs_count[i]; ++j) {
            display_router(&(tree->child[i][j]), depth + 1);
        }
    }
    if (tree->default_child != NULL) {
        log_info("%*sdefault", depth * 2, "");
        display_router(tree->default_child, depth + 1);
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
            error = log_error("Route has invalid path and will be ignored");
        if (routes[i].handler == NULL)
            error = log_error("Route has no handler and will be ignored");
        if (routes[i].method == NULL || test_route_method_validity(&routes[i]) != EXIT_SUCCESS)
            error = log_error("Route has no method or invalid method and will be ignored");
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

router_t *router_init(route_t routes[], size_t nb_routes)
{
    struct __route_tree_s *root = malloc(sizeof(struct __route_tree_s));

    if (root == NULL) {
        return NULL;
    }
    log_info("Checking routes validity");
    nb_routes = remove_route_uninitializable(routes, nb_routes);
    set_empty_tree_node(root);
    root->path = "/";
    root->path_len = 1;
    for (size_t i = 0; i < nb_routes; ++i) {
        if (routes[i].path[1] == '\0') {
            root->handler[get_method(routes[i].method)] = routes[i].handler;
        } else {
            router_add_route((void *)root, &(routes[i]));
        }
    }
    display_router(root, 0);
    return (router_t *)root;
}

int hash(char const *path, int size)
{
    int hash = 0;

    for (int i = 0; path[i] != '\0' && i < size; ++i) {
        hash += path[i] + i;
        hash = hash % HTTP_ROUTE_CHILD_COUNT;
    }
    return hash;
}

void set_empty_tree_node(struct __route_tree_s *tree)
{
    tree->default_child = NULL;
    memset(tree->handler, 0, sizeof(tree->handler));
    memset(tree->child, 0, sizeof(tree->child));
    memset(tree->childs_count, 0, sizeof(tree->childs_count));
    tree->path = NULL;
}

void router_destroy(router_t *tree)
{
    struct __route_tree_s *root = (struct __route_tree_s *)tree;

    if (root == NULL) {
        return;
    }
    for (size_t i = 0; i < HTTP_ROUTE_CHILD_COUNT; ++i) {
        for (size_t j = 0; j < root->childs_count[i]; ++j) {
            router_destroy((router_t)(&(root->child[i][j])));
        }
    }
    router_destroy((router_t)((root->default_child)));
    free(root);
}
