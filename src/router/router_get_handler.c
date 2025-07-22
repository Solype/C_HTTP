#include "http_log.h"
#include "router/router.h"
#include "router/router_utils.h"
#include "http_method.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static int does_child_exist(struct __route_tree_s *children, size_t n_chldrn, char const *path, size_t const path_len)
{
    if (n_chldrn == 0) {
        return -1;
    }
    for (size_t i = 0; i < n_chldrn; ++i) {
        if (strncmp(children[i].path, path, path_len) == 0) {
            return i;
        }
    }
    return -1;
}

static struct __route_tree_s *router_default_child_handler(
    struct __route_tree_s *tree, char const *path, size_t const path_len,
    struct handler_env_s *env)
{
    if (env->env_len == env->argc) {
        env->argv = realloc(env->argv, sizeof(char *) * (env->env_len + 1));
        if (env->argv == NULL) {
            log_error("Failed to allocate memory");
            return NULL;
        }
        env->argv_len = realloc(env->argv_len, sizeof(size_t) * (env->env_len + 1));
        if (env->argv_len == NULL) {
            log_error("Failed to allocate memory");
            return NULL;
        }
        env->env_len++;
    }
    env->argv[env->argc] = (char *)path + 1;
    env->argv_len[env->argc] = path_len - 1;
    env->argc++;
    return tree->default_child;
}

int handler_env_destroy(struct handler_env_s *env)
{
    free(env->argv);
    free(env->argv_len);
    env->argv = NULL;
    env->argv_len = NULL;
    env->argc = 0;
    env->env_len = 0;
    return 0;
}

int handler_env_init(struct handler_env_s *env)
{
    env->argv = NULL;
    env->argv_len = NULL;
    env->argc = 0;
    env->env_len = 0;
    return 0;
}

static handler_t finally_get_handler(struct __route_tree_s *root, char const *path, enum method_e method)
{
    int index = hash(path, strlen(path));
    int subindex = does_child_exist(root->child[index], root->childs_count[index], path, strlen(path));

    return (subindex == -1) ? NULL : root->child[index][subindex].handler[method];
}

static handler_t router_search(struct __route_tree_s *tree, char const *path, enum method_e method, struct handler_env_s *env)
{
    char const *save_pointer;
    char *slash_addr;
    int index;
    int tree_path_len;
    int subindex;

    while (1) {
        save_pointer = path;
        slash_addr = strchr(path + 1, '/');
        if (slash_addr == NULL)
            return finally_get_handler(tree, path, method);
        path = slash_addr;
        tree_path_len = ((intptr_t)save_pointer > (intptr_t)path) ? save_pointer - path : path - save_pointer;
        index = hash(save_pointer, tree_path_len);
        subindex = does_child_exist(tree->child[index], tree->childs_count[index], save_pointer, tree_path_len);
        tree = (subindex == -1) ? router_default_child_handler(tree, save_pointer, tree_path_len, env) : &(tree->child[index][subindex]);
        if (tree == NULL)
            return NULL;
    }
}

handler_t router_get_handler(router_t *tree, char const *path, enum method_e method, struct handler_env_s *env)
{
    struct __route_tree_s *root = (struct __route_tree_s *)tree;

    if (root == NULL) {
        log_error("Invalid router, router NULL, can't get handler");
        return NULL;
    }
    if (method == ERROR) {
        log_error("Invalid method, can't get handler");
        return NULL;
    }
    if (path[0] == '/' && path[1] == '\0')
        return root->handler[method];
    env->argc = 0;
    return router_search(root, path, method, env);
}
