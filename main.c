#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "http_server.h"
#include "router.h"

#define PORT 8080

static route_t routes[] = {
    {.handler = NULL, .method = NULL, .path = "/"},
    {.handler = NULL, .method = "GET", .path = "/api"},
    {.handler = NULL, .method = NULL, .path = "/aa"},
    {.handler = NULL, .method = "GEazezaea", .path = "/api/coucou"},
    {.handler = NULL, .method = "GET", .path = "/bb"},
    {.handler = NULL, .method = "GET", .path = "/bb/ping"},
};

int main() {
    // struct http_server_s server;
    // http_server_init(&server, PORT, 5);
    // http_server_enable_clean_quit(&server);
    // http_server_run(&server);
    // http_server_destroy(&server);
    router_t router = router_init(routes, sizeof(routes) / sizeof(route_t));
    router_destroy(router);
    return 0;
}
