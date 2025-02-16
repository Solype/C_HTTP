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

static void set_empty_tree_node(struct __route_tree_s *tree)
{
    tree->default_child = NULL;
    memset(tree->handler, 0, sizeof(tree->handler));
    memset(tree->child, 0, sizeof(tree->child));
    memset(tree->childs_count, 0, sizeof(tree->childs_count));
    tree->default_childs_count = 0;
    tree->path = NULL;
}

static int check_if_child_already_exists(struct __route_tree_s *tree, int index, char const *path, size_t const path_len)
{
    if (tree->childs_count[index] == 0) {
        return -1;
    }
    for (size_t i = 0; i < tree->childs_count[index]; ++i) {
        log_info("Checking %s against %s", tree->child[index][i].path, path);
        if (strncmp(tree->child[index][i].path, path, path_len) == 0) {
            return i;
        }
    }
    return -1;
}

static int insert_new_child(struct __route_tree_s *tree, int index, char *path, size_t const path_len)
{
    int subindex = check_if_child_already_exists(tree, index, path, path_len);

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

static int insert_route(struct __route_tree_s *tree, route_t *route)
{
    (void)tree;
    char *save_pointer = route->path;
    char *slash_addr = strchr(route->path + 1, '/');
    int index;
    int subindex;
    
    if (slash_addr == NULL) {
        index = hash(route->path, strlen(route->path));
        subindex = insert_new_child(tree, index, save_pointer, strlen(route->path));
        if (subindex == -1) {
            return EXIT_FAILURE;
        }
    } else {
        route->path = slash_addr;
        subindex = ((intptr_t)save_pointer > (intptr_t)route->path) ? save_pointer - route->path : route->path - save_pointer;
        index = hash(save_pointer, subindex);
        subindex = insert_new_child(tree, index, save_pointer, subindex);
        if (subindex == -1) {
            return EXIT_FAILURE;
        }
        insert_route(&(tree->child[index][tree->childs_count[index] - 1]), route);
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
    memset(root->childs_count, 0, sizeof(root->childs_count));
    root->default_childs_count = 0;
    root->path = "/";
    root->path_len = 1;
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
    struct __route_tree_s *root = (struct __route_tree_s *)tree;

    if (root == NULL) {
        return;
    }
    for (size_t i = 0; i < HTTP_ROUTE_CHILD_COUNT; ++i) {
        for (size_t j = 0; j < root->childs_count[i]; ++j) {
            router_destroy((router_t)(&(root->child[i][j])));
        }
    }
    for (size_t i = 0; i < root->default_childs_count; ++i) {
        router_destroy((router_t)(&(root->default_child[i])));
    }
    free(root);
}


