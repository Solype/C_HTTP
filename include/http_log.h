/*
** EPITECH PROJECT, 2025
** C_HTTP
** File description:
** log
*/

#ifndef HTTP_LOG_H_
    #define HTTP_LOG_H_


    #define RED "\033[1;31m"
    #define GREEN "\033[1;32m"
    #define YELLOW "\033[1;33m"
    #define BLUE "\033[1;34m"
    #define RESET "\033[0m"


int log_success(const char *message, ...);
int log_error(const char *message, ...);
int log_warning(const char *message, ...);
int log_info(const char *message, ...);
int log_space(void);

#endif /* !HTTP_LOG_H_ */
