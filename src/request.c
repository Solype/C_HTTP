#include "request.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int request_init(struct request_s *request, char *raw_request)
{
    request->raw_request = raw_request;
    request->method = strtok(request->raw_request, " ");
    request->uri = strtok(NULL, " ");
    request->version = strtok(NULL, "\r\n");
    request->rest = strtok(NULL, "");
    return 0;
}

int request_destroy(struct request_s *request)
{
    free(request->raw_request);
    return 0;
}

int request_display(struct request_s *request)
{
    printf("Request:\n");
    printf("Method: %s\n", request->method);
    printf("URI: %s\n", request->uri);
    printf("Version: %s\n", request->version);
    printf("Rest: %s\n", request->rest);
    return 0;
}
