#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
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

static int handler_a(struct request_s *request)
{
    (void)request;
    printf("ROUTE 'a'\n");
    return 0;
}

static int handler_b(struct request_s *request)
{
    (void)request;
    printf("ROUTE 'b'\n");
    return 0;
}

static int handler_c(struct request_s *request)
{
    (void)request;
    printf("ROUTE 'c'\n");
    return 0;
}

static int handler_d(struct request_s *request)
{
    (void)request;
    printf("ROUTE 'd'\n");
    return 0;
}

static int handler_e(struct request_s *request)
{
    (void)request;
    printf("ROUTE 'e'\n");
    return 0;
}

static int handler_f(struct request_s *request)
{
    (void)request;
    printf("ROUTE 'f'\n");
    return 0;
}

static route_t routes[] = {
    {.handler = handler_a, .method = "GET", .path = a},
    {.handler = handler_b, .method = "GET", .path = b},
    {.handler = handler_c, .method = "GET", .path = c},
    {.handler = handler_d, .method = "GET", .path = d},
    {.handler = handler_e, .method = "GET", .path = e},
    {.handler = handler_f, .method = "GET", .path = f},
};


int main() {
    // struct http_server_s server;
    // http_server_init(&server, PORT, 5);
    // http_server_enable_clean_quit(&server);
    // http_server_run(&server);
    // http_server_destroy(&server);
    router_t router = router_init(routes, sizeof(routes) / sizeof(route_t));
    handler_t h = router_get_handler(router, "GET", a); // Utilise le bon type
    h(NULL);
    h = router_get_handler(router, "GET", b);
    h(NULL);
    h = router_get_handler(router, "GET", c);
    h(NULL);
    h = router_get_handler(router, "GET", d);
    h(NULL);
    h = router_get_handler(router, "GET", e);
    h(NULL);
    h = router_get_handler(router, "GET", f);
    h(NULL);
    router_destroy(router);
    return 0;
}
