#include "http_log.h"
#include "router.h"
#include "router_utils.h"
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

static int check_if_child_already_exists(struct __route_tree_s *children, size_t nb_child, char const *path, size_t const path_len)
{
    if (nb_child == 0) {
        return -1;
    }
    for (size_t i = 0; i < nb_child; ++i) {
        if (strncmp(children[i].path, path, path_len) == 0) {
            return i;
        }
    }
    return -1;
}

static int insert_new_child(struct __route_tree_s *tree, int index, char *path, size_t const path_len)
{
    int subindex = check_if_child_already_exists(
        tree->child[index], tree->childs_count[index], path, path_len
    );

    if (subindex != -1) {
        return subindex;
    }
    subindex = tree->childs_count[index];
    ++tree->childs_count[index];
    tree->child[index] = realloc(tree->child[index], sizeof(struct __route_tree_s) * (subindex + 1));
    if (tree->child[index] == NULL) {
        return -log_error("Failed to allocate memory");
    }
    set_empty_tree_node(&(tree->child[index][subindex]));
    tree->child[index][subindex].path = path;
    tree->child[index][subindex].path_len = path_len;
    return subindex;
}

static struct __route_tree_s *check_default_child(struct __route_tree_s *tree, route_t *route)
{
    (void)tree;

    if (strncmp(route->path, "/*/", 3) == 0) {
        route->path += 2;
        if (tree->default_child != NULL) {
            return tree->default_child;
        }
        tree->default_child = malloc(sizeof(struct __route_tree_s));
        set_empty_tree_node(tree->default_child);
        return tree->default_child;
    }
    return NULL;
}

static int finally_insert_handler(struct __route_tree_s *tree, route_t *route)
{
    int index = hash(route->path, strlen(route->path));
    int subindex = insert_new_child(tree, index, route->path, strlen(route->path));

    if (subindex == -1) {
        return EXIT_FAILURE;
    }
    tree->child[index][subindex].handler[get_method(route->method)] = route->handler;
    return EXIT_SUCCESS;
}

static struct __route_tree_s *dive_deeper_in_the_tree(struct __route_tree_s *tree, route_t *route, char *save_pointer)
{
    int subindex = ((intptr_t)save_pointer > (intptr_t)route->path) ? save_pointer - route->path : route->path - save_pointer;
    int index = hash(save_pointer, subindex);

    subindex = insert_new_child(tree, index, save_pointer, subindex);
    if (subindex == -1) {
        return NULL;
    }
    return &(tree->child[index][tree->childs_count[index] - 1]);
}

static int insert_route(struct __route_tree_s *tree, route_t *route)
{
    char *save_pointer;
    char *slash_addr;
    struct __route_tree_s *tmp = NULL;

    while (1) {
        save_pointer = route->path;
        slash_addr = strchr(route->path + 1, '/');
        tmp = check_default_child(tree, route);
        if (tmp != NULL) {
            tree = tmp;
            continue;
        }
        if (slash_addr == NULL) {
            return finally_insert_handler(tree, route);
        }
        route->path = slash_addr;
        tree = dive_deeper_in_the_tree(tree, route, save_pointer);
        if (tree == NULL) {
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

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

int router_add_route(router_t *tree, route_t *route)
{
    if (tree == NULL) {
        return log_error("Router is not initialized");
    }
    if (route == NULL) {
        return log_error("Route is not initialized");
    }
    return insert_route((struct __route_tree_s *)tree, route);
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
            insert_route(root, &(routes[i]));
        }
    }
    display_router(root, 0);
    return (router_t *)root;
}


