#include "request.h"
#include "http_method.h"

#ifndef ROUTE_TREE_H_
    #define ROUTE_TREE_H_

    #define HTTP_ROUTE_CHILD_COUNT 40

typedef int (*handler_t)(struct request_s *request);


struct handler_env_s {
    char **argv;
    size_t *argv_len;
    size_t argc;
    size_t env_len;
};


struct __route_tree_s {
    char *path;
    size_t path_len;
    struct __route_tree_s *child[HTTP_ROUTE_CHILD_COUNT];
    struct __route_tree_s *default_child;
    size_t childs_count[HTTP_ROUTE_CHILD_COUNT];
    handler_t handler[NB_METHODS];
};


typedef struct route_s {
    char *path;
    char *method;
    handler_t handler;
} route_t;


typedef void *router_t;

/**
 * @brief Initialize a route tree
 * 
 * @param routes An array of routes
 * @param nb_routes The number of routes
 * @return A pointer to the route tree
 */
router_t *router_init(route_t routes[], size_t nb_routes);

/**
 * @brief Destroy a route tree
 * 
 * @param tree The route tree to destroy
 */
void router_destroy(router_t *tree);

/**
 * @brief Get a handler from a route tree
 * 
 * @param tree The route tree to get the handler from
 * @param path The path of the request
 * @param method The method of the request
 * @return The handler of the request
 */
handler_t router_get_handler(router_t *tree, char const *path, enum method_e method, struct handler_env_s *env);

/**
 * @brief Add a route to a route tree
 * 
 * @param tree The route tree to add the route to
 * @param route The route to add
 */
int router_add_route(router_t *tree, route_t *route);

/**
 * @brief Destroy the handler environment
 * 
 * @param env The handler environment to destroy
 * @return 0 on success, 1 on failure, see macro EXIT_FAILURE and EXIT_SUCCESS
 */
int handler_env_destroy(struct handler_env_s *env);

/**
 * @brief Initialize the handler environment
 * 
 * @param env The handler environment to initialize
 * @return 0 on success, 1 on failure, see macro EXIT_FAILURE and EXIT_SUCCESS
 */
int handler_env_init(struct handler_env_s *env);

#endif /* !ROUTE_TREE_H_ */
