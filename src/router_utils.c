#include "http_log.h"
#include "router.h"
#include "http_method.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

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
