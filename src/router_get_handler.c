#include "http_log.h"
#include "router.h"
#include "router_utils.h"
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

static struct __route_tree_s *router_check_default_child_for_handler(
    struct __route_tree_s *tree, char const *path, size_t const path_len)
{
    log_info("Checking default child %.*s", (int)path_len, path);
    return tree->default_child;
}

handler_t router_get_handler(router_t *tree, char const *method, char const *path)
{
    struct __route_tree_s *root = (struct __route_tree_s *)tree;
    char const *save_pointer;
    char *slash_addr;
    int index;
    int tree_path_len;
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
        tree_path_len = ((intptr_t)save_pointer > (intptr_t)path) ? save_pointer - path : path - save_pointer;
        index = hash(save_pointer, tree_path_len);
        subindex = check_if_child_already_exists(root->child[index], root->childs_count[index], save_pointer, tree_path_len);
        if (subindex == -1) {
            root = router_check_default_child_for_handler(root, save_pointer, tree_path_len);
        } else {
            root = &(root->child[index][subindex]);
        }
    }
    return NULL;
}
