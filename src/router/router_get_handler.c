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

static struct __route_tree_s *get_child_normal(struct __route_tree_s *tree, char const *path, size_t len)
{
    size_t index = hash(path, len);

    for (size_t i = 0; i < tree->children_len[index]; i++) {
        if (strncmp(tree->children[index][i].path, path, len) == 0) {
            return &tree->children[index][i];
        }
    }
    return NULL;
}

static void env_add_whild_card(struct handler_env_s * env, char const *path, size_t len)
{
    if (env->argc == env->env_len) {
        env->argv = realloc(env->argv, sizeof(char *) * (env->env_len + 1));
        env->argv_len = realloc(env->argv_len, sizeof(size_t) * (env->env_len + 1));
        env->env_len += 1;
    }
    env->argv[env->argc] = (char *)path;
    env->argv_len[env->argc] = len;
    env->argc += 1;
}

static int get_slash_offset(char const *path)
{
    int i = 0;

    while (path[i] == '/') {
        i += 1;
    }
    return i;
}

static handler_t router_search(struct __route_tree_s *tree, char const *path, enum method_e method, struct handler_env_s *env)
{
    char *slash_addr ;
    long int current_path_node_len;
    struct __route_tree_s *child;
    int slash_offset;

    while (1) {
        slash_offset = get_slash_offset(path);
        slash_addr = strchr(path + slash_offset, '/');
        current_path_node_len = (slash_addr == NULL) ? strlen(path) : (long int)(path - slash_addr);
        current_path_node_len = (current_path_node_len < 0) ? -current_path_node_len : current_path_node_len;
        current_path_node_len -= slash_offset;
        child = get_child_normal(tree, path + slash_offset, current_path_node_len);
        if (child == NULL) {
            tree = tree->default_children;
            env_add_whild_card(env, path + slash_offset, current_path_node_len);
        } else {
            tree = child;
        }
        if (tree == NULL) {
            return NULL;
        }
        if (slash_addr == NULL) {
            return tree->handler[method];
        }
        path = slash_addr;
    }
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///     PUBLIC
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

int handler_env_destroy(struct handler_env_s *env)
{
    if (env->argv != NULL) {
        free(env->argv);
        log_success("array of params freed !");
    }
    if (env->argv_len != NULL) {
        free(env->argv_len);
        log_success("array of sizes freed !");
    }
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
    if (path[0] == '/' && path[1] == '\0') {
        return root->handler[method];
    }
    env->argc = 0;
    return router_search(root, path, method, env);
}
