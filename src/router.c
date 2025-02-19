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

static int hash(char const *path, int size)
{
    int hash = 0;

    for (int i = 0; path[i] != '\0' && i < size; ++i) {
        hash += path[i] + i;
        hash = hash % HTTP_ROUTE_CHILD_COUNT;
    }
    return hash;
}

static void set_empty_tree_node(struct __route_tree_s *tree)
{
    tree->default_child = NULL;
    memset(tree->handler, 0, sizeof(tree->handler));
    memset(tree->child, 0, sizeof(tree->child));
    memset(tree->childs_count, 0, sizeof(tree->childs_count));
    tree->path = NULL;
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
    // int subbindex;
    // char *next_slash;
    (void)tree;

    log_info("Checking default child %s", route->path);
    if (strncmp(route->path, "/*/", 3) == 0) {
        log_warning("found default child");
        route->path += 2;
        log_error("Path is now %s", route->path);
        if (tree->default_child != NULL) {
            return tree->default_child;
        }
        tree->default_child = malloc(sizeof(struct __route_tree_s));
        set_empty_tree_node(tree->default_child);
        return tree->default_child;
        // tree->default_child = realloc(tree->default_child, sizeof(struct __route_tree_s) * (len + 1));
        // if (tree->default_child == NULL) {
        //     return -log_error("Failed to allocate memory");
        // }
        // set_empty_tree_node(&(tree->default_child[len]));
        // tree->default_child[len].path = route->path;
        // tree->default_child[len].path_len = strlen(route->path) - 2;
        // tree->default_childs_count = len + 1;
        // return EXIT_SUCCESS;
    }
    return NULL;
}

static int insert_route(struct __route_tree_s *tree, route_t *route)
{
    char *save_pointer;
    char *slash_addr;
    int index;
    int subindex;
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
            index = hash(route->path, strlen(route->path));
            subindex = insert_new_child(tree, index, save_pointer, strlen(route->path));
            if (subindex == -1) {
                return EXIT_FAILURE;
            }
            tree->child[index][subindex].handler[get_method(route->method)] = route->handler;
            return EXIT_SUCCESS;
        }
        route->path = slash_addr;
        subindex = ((intptr_t)save_pointer > (intptr_t)route->path) ? save_pointer - route->path : route->path - save_pointer;
        index = hash(save_pointer, subindex);
        subindex = insert_new_child(tree, index, save_pointer, subindex);
        if (subindex == -1) {
            return EXIT_FAILURE;
        }
        tree = &(tree->child[index][tree->childs_count[index] - 1]);
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

handler_t router_get_handler(router_t *tree, char const *method, char const *path)
{
    struct __route_tree_s *root = (struct __route_tree_s *)tree;
    char const *save_pointer;
    char *slash_addr;
    int index;
    int subindex;

    if (path[0] == '/' && path[1] == '\0') {
        return root->handler[get_method(method)];
    }
    while (1) {
        save_pointer = path;
        slash_addr = strchr(path + 1, '/');
        if (slash_addr == NULL) {
            index = hash(path, strlen(path));
            subindex = check_if_child_already_exists(root->child[index], root->childs_count[index], path, strlen(path));
            return (subindex == -1) ? NULL : root->child[index][subindex].handler[get_method(method)];
        }
        path = slash_addr;
        subindex = ((intptr_t)save_pointer > (intptr_t)path) ? save_pointer - path : path - save_pointer;
        index = hash(save_pointer, subindex);
        subindex = check_if_child_already_exists(root->child[index], root->childs_count[index], save_pointer, subindex);
        if (subindex == -1) return NULL;
        root = &(root->child[index][subindex]);
    }
    return NULL;
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


