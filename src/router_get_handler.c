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
        log_warning("No children");
        return -1;
    }
    for (size_t i = 0; i < nb_child; ++i) {
        log_info("Checking child %.*s with path %.*s", (int)children[i].path_len, children[i].path, (int)path_len, path);
        if (strncmp(children[i].path, path, path_len) == 0) {
            return i;
        }
    }
    return -1;
}

static struct __route_tree_s *router_check_default_child_for_handler(
    struct __route_tree_s *tree, char const *path, size_t const path_len,
    struct handler_env_s *env)
{
    log_info("Checking default child %.*s, inserting it in env", (int)path_len, path);
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

handler_t router_get_handler(router_t *tree, char const *path, char const *method, struct handler_env_s *env)
{
    struct __route_tree_s *root = (struct __route_tree_s *)tree;
    char const *save_pointer;
    char *slash_addr;
    int index;
    int tree_path_len;
    int subindex;

    log_info("Getting handler for method '%s' and path '%s'", method, path);
    if (path[0] == '/' && path[1] == '\0') {
        log_info("Found root handler");
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
            root = router_check_default_child_for_handler(root, save_pointer, tree_path_len, env);
        } else {
            root = &(root->child[index][subindex]);
        }
        if (root == NULL) {
            return NULL;
        }
    }
    return NULL;
}
