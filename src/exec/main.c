#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "http_server.h"
#include "router.h"
#include "http_log.h"

#define PORT 8080

__attribute__((section(".data"))) static char a[] = "/";
__attribute__((section(".data"))) static char b[] = "/api/1";
__attribute__((section(".data"))) static char c[] = "/bb/ping";
__attribute__((section(".data"))) static char d[] = "/bb/";
__attribute__((section(".data"))) static char e[] = "/bb/aa/cc/dd";
__attribute__((section(".data"))) static char f[] = "/api/*/coucou_les_amis/test";

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

static char *routes_names[] = {
    a, b, c, d, e, "/api/efeadfezfrezf/coucou_les_amis/test", "/api/aze/coucou_les_amis/test",
};

#define NB_ROUTES (sizeof(routes) / sizeof(route_t))

int main() {
    // struct http_server_s server;
    // http_server_init(&server, PORT, 5);
    // http_server_enable_clean_quit(&server);
    // http_server_run(&server);
    // http_server_destroy(&server);
    router_t router = router_init(routes, NB_ROUTES);
    struct handler_env_s env;
    handler_t h;

    handler_env_init(&env);
    for (size_t i = 0; i < NB_ROUTES + 1; ++i) {
        h = router_get_handler(router, routes_names[i], "GET", &env);
        if (h != NULL) {
            h(NULL);
            for (size_t j = 0; j < env.argc; ++j) {
                log_info("Arg %ld: %.*s", j, env.argv_len[j], env.argv[j]);
            }
        } else {
            log_error("Route '%s' not found", routes_names[i]);
        }
    }
    router_destroy(router);
    handler_env_destroy(&env);
    // printf("%ld\n", sizeof(void *));
    return 0;
}
