#include "utils/http_log.h"
#include "router/router.h"
#include "router/router_utils.h"
#include "utils/http_method.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///     PRIVATE
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

static int get_slash_offset(char const *path)
{
    int i = 0;

    while (path[i] == '/') {
        i += 1;
    }
    return i;
}


static struct __route_tree_s *get_child(struct __route_tree_s *current_node, route_t *route)
{
    int slash_off_set = get_slash_offset(route->path);
    char *true_path = route->path + slash_off_set;
    char *next_slash = strchr(true_path, '/');
    ptrdiff_t diff = (next_slash == NULL) ? (ptrdiff_t)(strlen(true_path)) : (next_slash - true_path);
    size_t len_current_text_node = diff < 0 ? (size_t)(-diff) : (size_t)diff;
    int index;
    int subindex;
    // Handling the default child;
    if (true_path[0] == '*' && (true_path[1] == 0 || true_path[1] == '/')) {
        if (current_node->default_children != NULL) {
            return current_node->default_children;
        }
        current_node->default_children = malloc(sizeof(struct __route_tree_s));
        if (current_node->default_children == NULL) {
            return NULL;
        }
        set_empty_tree_node(current_node->default_children);
        return current_node->default_children;
    }

    // handling the normal nodes
    index = hash(true_path, len_current_text_node);
    for (size_t i = 0; i < current_node->children_len[index]; ++i) {
        if (strncmp(current_node->children[index][i].path, true_path, len_current_text_node) == 0) {
            return &current_node->children[index][i];
        }
    }
    subindex = current_node->children_len[index];
    current_node->children_len[index] += 1;
    current_node->children[index] = realloc(current_node->children[index], sizeof(struct __route_tree_s) * current_node->children_len[index]);
    set_empty_tree_node(&current_node->children[index][subindex]);
    current_node->children[index][subindex].path = true_path;
    current_node->children[index][subindex].path_len = len_current_text_node;
    return &current_node->children[index][subindex];
}

static int insert_route(struct __route_tree_s *tree, route_t *route)
{
    char *slash_addr;

    while (1) {
        if (route->path == NULL) {
            tree->handler[get_method(route->method)] = route->handler;
            return EXIT_SUCCESS;
        }
        tree = get_child(tree, route);
        if (tree == NULL) {
            return EXIT_FAILURE;
        }
        slash_addr = strchr(route->path + 1, '/');
        route->path = slash_addr;
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
    if (strcmp("/", route->path) == 0) {
        struct __route_tree_s *root = (struct __route_tree_s *)tree;
        root->handler[get_method(route->method)] = route->handler;
        log_info("setting a root handler at : %s", route->path);
        return EXIT_SUCCESS;
    }
    log_info("inserting : %s", route->path);
    return insert_route((struct __route_tree_s *)tree, route);
}

