#include <arpa/inet.h>

#ifndef HTTP_SERVER_H_
    #define HTTP_SERVER_H_


typedef enum {
    true,
    false
} bool;

/**
 * @struct http_server
 * @brief Structure representing an HTTP server
 */
struct http_server_s {
    int socket;
    struct sockaddr_in addr;
    bool clean_quit;
};

/**
 * @brief Initialize an HTTP server
 * 
 * @param server The server to initialize
 * @param port The port to listen on
 */
int http_server_init(struct http_server_s *server, int port, int nb_clients);

/**
 * @brief Run the HTTP server
 * 
 * @param server The server to run
 */
int http_server_run(struct http_server_s *server);

/**
 * @brief Enable clean quit for the HTTP server
 * 
 * @param server The server to enable clean quit for
 */
int http_server_enable_clean_quit(struct http_server_s *server);

/**
 * @brief Destroy an HTTP server
 * 
 * @param server The server to destroy
 * @return int 0 on success, 1 on failure
 */
int http_server_destroy(struct http_server_s *server);


#endif /* !HTTP_SERVER_H_ */
