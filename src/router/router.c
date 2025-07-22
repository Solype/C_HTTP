#include "http_log.h"
#include "router/router.h"
#include "router/router_utils.h"
#include "http_method.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

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


