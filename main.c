#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "http_server.h"
#include "router.h"

#define PORT 8080

__attribute__((section(".data"))) static char a[] = "/";
__attribute__((section(".data"))) static char b[] = "/api/1";
__attribute__((section(".data"))) static char c[] = "/aa/ping";
__attribute__((section(".data"))) static char d[] = "/bb/ping";
__attribute__((section(".data"))) static char e[] = "/bb/aa";
__attribute__((section(".data"))) static char f[] = "/api/coucou_les_amis";

static route_t routes[] = {
    {.handler = NULL, .method = "GET", .path = a},
    {.handler = NULL, .method = "GET", .path = b},
    {.handler = NULL, .method = "GET", .path = c},
    {.handler = NULL, .method = "GET", .path = d},
    {.handler = NULL, .method = "GET", .path = e},
    {.handler = NULL, .method = "GET", .path = f},
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
