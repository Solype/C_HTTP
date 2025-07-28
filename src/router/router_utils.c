#include "utils/http_log.h"
#include "router/router.h"
#include "router/router_utils.h"
#include "utils/http_method.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// static void display_router(struct __route_tree_s *tree, size_t depth)
// {

//     if (tree->path != NULL && tree->path_len > 0 && tree->path[0] != '\0') {
//         log_info("%*s%.*s", depth * 2, "", (int)tree->path_len, tree->path);
//     }
//     for (size_t i = 0; i < NB_METHODS; ++i) {
//         if (tree->handler[i] != NULL) {
//             log_info("%*s%s", depth * 2, "", get_method_name(i));
//         }
//     }
//     for (size_t i = 0; i < HTTP_ROUTE_CHILD_COUNT; ++i) {
//         for (size_t j = 0; j < tree->children_len[i]; ++j) {
//             display_router(&(tree->children[i][j]), depth + 1);
//         }
//     }
//     if (tree->default_children != NULL) {
//         log_info("%*sdefault", depth * 2, "");
//         display_router(tree->default_children, depth + 1);
//     }
// }

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///     PRIVATE
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


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
            error = log_error("A route has invalid path and will be ignored, "
                    "please make sure your root start with / and does not have any space");
        if (routes[i].handler == NULL)
            error = log_error("A route has no handler and will be ignored");
        if (routes[i].method == NULL || test_route_method_validity(&routes[i]) != EXIT_SUCCESS)
            error = log_error("A route has no method or invalid method and will be ignored");
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

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///     PUBLIC
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

router_t *router_init(route_t *routes, size_t nb_routes)
{
    struct __route_tree_s *root = malloc(sizeof(struct __route_tree_s));

    if (root == NULL) {
        return NULL;
    }
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
    return (router_t *)root;
}

static void recursiv_router_destroy(struct __route_tree_s *tree)
{
    if (tree == NULL) {
        return;
    }
    for (size_t i = 0; i < HTTP_ROUTE_CHILD_COUNT; ++i) {
        for (size_t j = 0; j < tree->children_len[i]; ++j) {
            recursiv_router_destroy(&(tree->children[i][j]));
        }
        free(tree->children[i]);
    }
    recursiv_router_destroy((tree->default_children));
    if (tree->default_children != NULL) {
        free(tree->default_children);
    }
}

void router_destroy(router_t *tree)
{
    struct __route_tree_s *root = (struct __route_tree_s *)tree;

    recursiv_router_destroy(root);
    free(root);
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
    tree->default_children = NULL;
    memset(tree->handler, 0, sizeof(tree->handler));
    memset(tree->children, 0, sizeof(tree->children));
    memset(tree->children_len, 0, sizeof(tree->children_len));
    tree->path = NULL;
}
