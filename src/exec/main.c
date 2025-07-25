#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "http_server.h"
#include "router/router.h"
#include "utils/http_log.h"

#define PORT 8080

__attribute__((section(".data"))) static char a[] = "/";
__attribute__((section(".data"))) static char b[] = "/api/1";
__attribute__((section(".data"))) static char c[] = "/bb/ping";
__attribute__((section(".data"))) static char d[] = "/bb/";
__attribute__((section(".data"))) static char e[] = "/api/*/";
__attribute__((section(".data"))) static char f[] = "/api/*/coucou_les_amis/test";

static int handler_a(struct request_s *request, struct handler_env_s *env, struct response_s *response)
{
    (void)request;
    (void)env;
    response->body = "{\"a\": \"a\"}";
    response->content_type = application_json;
    response->status_code = 403;
    printf("ROUTE 'a'\n");
    return 0;
}

static int handler_b(struct request_s *request, struct handler_env_s *env, struct response_s *response)
{
    (void)request;
    (void)response;
    (void)env;
    printf("ROUTE 'b'\n");
    return 0;
}

static int handler_c(struct request_s *request, struct handler_env_s *env, struct response_s *response)
{
    (void)request;
    (void)response;
    (void)env;
    printf("ROUTE 'c'\n");
    return 0;
}

static int handler_d(struct request_s *request, struct handler_env_s *env, struct response_s *response)
{
    (void)request;
    (void)response;
    (void)env;
    printf("ROUTE 'd'\n");
    return 0;
}

static int handler_e(struct request_s *request, struct handler_env_s *env, struct response_s *response)
{
    (void)request;
    (void)response;
    (void)env;
    response->body_size = snprintf(response->body, 1024, "<html><body><h1>Hello !</h1><p>I am %s</p></body></html>", env->argv[0]);
    response->content_type = text_html;
    printf("ROUTE 'e'\n");
    return 0;
}

static int handler_f(struct request_s *request, struct handler_env_s *env, struct response_s *response)
{
    (void)request;
    (void)env;
    response->body_size = snprintf(response->body, 1024, "{\"params\": \"%.*s\"}", (int)env->argv_len[0], env->argv[0]);
    response->content_type = application_json;
    printf("ROUTE 'f' %ld\n", env->argc);
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

#define NB_ROUTES (sizeof(routes) / sizeof(route_t))

int main() {
    router_t router = router_init(routes, NB_ROUTES);

    struct http_server_s *server = http_server_create(PORT, 5);
    http_server_enable_clean_quit(server);
    http_server_run(server, router);
    http_server_destroy(server);
    router_destroy(router);
    return 0;
}
