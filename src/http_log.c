#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "http_log.h"


int log_success(const char *message, ...)
{
    va_list args;

    va_start(args, message);
    fprintf(stderr, "[" GREEN "SUCCESS" RESET "] ");
    vfprintf(stderr, message, args);
    fprintf(stderr, "\n");
    va_end(args);
    return EXIT_SUCCESS;
}

int log_error(const char *message, ...)
{
    va_list args;

    va_start(args, message);
    fprintf(stderr, "[" RED " ERROR "  RESET "] ");
    vfprintf(stderr, message, args);
    fprintf(stderr, "\n");
    va_end(args);
    return EXIT_FAILURE;
}

int log_warning(const char *message, ...)
{
    va_list args;

    va_start(args, message);
    fprintf(stderr, "[" YELLOW "WARNINGS" RESET "] ");
    vfprintf(stderr, message, args);
    fprintf(stderr, "\n");
    va_end(args);
    return EXIT_SUCCESS;
}

int log_info(const char *message, ...)
{
    va_list args;

    va_start(args, message);
    fprintf(stderr, "[" BLUE " INFOS " RESET "] ");
    vfprintf(stderr, message, args);
    fprintf(stderr, "\n");
    va_end(args);
    return EXIT_SUCCESS;
}

