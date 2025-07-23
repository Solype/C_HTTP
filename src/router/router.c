#include "utils/http_log.h"
#include "router/router.h"
#include "router/router_utils.h"
#include "utils/http_method.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///     PRIVATE
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


static int check_if_children_already_exists(struct __route_tree_s *childrenren, size_t nb_children, char const *path, size_t const path_len)
{
    if (nb_children == 0) {
        return -1;
    }
    for (size_t i = 0; i < nb_children; ++i) {
        if (strncmp(childrenren[i].path, path, path_len) == 0) {
            return i;
        }
    }
    return -1;
}

static int insert_new_children(struct __route_tree_s *tree, int index, char *path, size_t const path_len)
{
    int subindex = check_if_children_already_exists(
        tree->children[index], tree->children_len[index], path, path_len
    );

    if (subindex != -1) {
        return subindex;
    }
    subindex = tree->children_len[index];
    ++tree->children_len[index];
    tree->children[index] = realloc(tree->children[index], sizeof(struct __route_tree_s) * (subindex + 1));
    if (tree->children[index] == NULL) {
        return -log_error("Failed to allocate memory");
    }
    set_empty_tree_node(&(tree->children[index][subindex]));
    tree->children[index][subindex].path = path;
    tree->children[index][subindex].path_len = path_len;
    return subindex;
}

static struct __route_tree_s *check_default_children(struct __route_tree_s *tree, route_t *route)
{
    (void)tree;

    if (strncmp(route->path, "/*/", 3) == 0) {
        route->path += 2;
        if (tree->default_children != NULL) {
            return tree->default_children;
        }
        tree->default_children = malloc(sizeof(struct __route_tree_s));
        set_empty_tree_node(tree->default_children);
        return tree->default_children;
    }
    return NULL;
}

static int finally_insert_handler(struct __route_tree_s *tree, route_t *route)
{
    int index = hash(route->path, strlen(route->path));
    int subindex = insert_new_children(tree, index, route->path, strlen(route->path));

    if (subindex == -1) {
        return EXIT_FAILURE;
    }
    tree->children[index][subindex].handler[get_method(route->method)] = route->handler;
    return EXIT_SUCCESS;
}

static struct __route_tree_s *dive_deeper_in_the_tree(struct __route_tree_s *tree, route_t *route, char *save_pointer)
{
    int subindex = ((intptr_t)save_pointer > (intptr_t)route->path) ? save_pointer - route->path : route->path - save_pointer;
    int index = hash(save_pointer, subindex);

    subindex = insert_new_children(tree, index, save_pointer, subindex);
    if (subindex == -1) {
        return NULL;
    }
    return &(tree->children[index][tree->children_len[index] - 1]);
}

static int insert_route(struct __route_tree_s *tree, route_t *route)
{
    char *save_pointer;
    char *slash_addr;
    struct __route_tree_s *tmp = NULL;

    while (1) {
        save_pointer = route->path;
        slash_addr = strchr(route->path + 1, '/');
        tmp = check_default_children(tree, route);
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

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///     PUBLIC
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


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


