#include "request.h"
#include "utils/http_method.h"
#include "response.h"

#ifndef ROUTE_TREE_H_
    #define ROUTE_TREE_H_

    #define HTTP_ROUTE_CHILD_COUNT 40


struct handler_env_s {
    char **argv;        // list of arguments
    size_t *argv_len;   // size of each argument
    size_t argc;        // numpber of argument
    size_t env_len;     // true size of the number of argv (to avoid reallocating every time)
};

typedef int (*handler_t)(struct request_s *request, struct handler_env_s *env, struct response_s *response);

struct __route_tree_s {
    char *path;
    size_t path_len;
    // This is an array of array, the length of each array contained on this array are
    // kept int the attribute children_len
    struct __route_tree_s *children[HTTP_ROUTE_CHILD_COUNT];
    struct __route_tree_s *default_children;
    size_t children_len[HTTP_ROUTE_CHILD_COUNT];
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
router_t *router_init(route_t *routes, size_t nb_routes);

/**
 * @brief Destroy a route tree
 *
 * @param tree The route tree to destroy
 */
void router_destroy(router_t *tree);

/**
 * @brief Get a handler from a route tree
 *
 * @param t The route tree to get the handler from
 * @param p The path of the request
 * @param m The method of the request
 * @param e The handler environment
 * @return The handler of the request
 */
handler_t router_get_handler(
    router_t *t, char const *p, enum method_e m, struct handler_env_s *e
);

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
