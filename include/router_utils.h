#include "router.h"

#ifndef ROUTER_UTILS_H_
    #define ROUTER_UTILS_H_

/**
 * @brief Hash a path
 * 
 * @param path The path to hash
 * @param size The size of the path
 * @return The hash of the path
 */
int hash(char const *path, int size);

/**
 * @brief Set a route tree node to empty
 *
 * @param tree The route tree node to set
 */
void set_empty_tree_node(struct __route_tree_s *tree);


#endif /* !ROUTER_UTILS_H_ */
