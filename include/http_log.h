
#ifndef HTTP_LOG_H_
    #define HTTP_LOG_H_


    #define RED "\033[1;31m"
    #define GREEN "\033[1;32m"
    #define YELLOW "\033[1;33m"
    #define BLUE "\033[1;34m"
    #define RESET "\033[0m"


/**
 * @brief Log a success message
 * 
 * @param message The message to log
 */
int log_success(const char *message, ...);

/**
 * @brief Log an error message
 * 
 * @param message The message to log
 */
int log_error(const char *message, ...);

/**
 * @brief Log a warning message
 * 
 * @param message The message to log
 */
int log_warning(const char *message, ...);

/**
 * @brief Log an info message
 * 
 * @param message The message to log
 */
int log_info(const char *message, ...);

/**
 * @brief Log a newline
 */
int log_space(void);

#endif /* !HTTP_LOG_H_ */
