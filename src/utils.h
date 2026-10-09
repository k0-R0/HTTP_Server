#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>

typedef struct {
    char method[16];
    char path[256];
    char version[16];
    const char *raw;
} HttpRequest;

int send_success(int sock_fd);
int send_failure(int sock_fd);
int get_client_request(int sock_fd, char *buffer, size_t buffer_len);
int send_http_response(int sock_fd, unsigned short status,
                       const char *content_type, const char *response_body,
                       size_t response_len);
int route_request(int client_fd, const HttpRequest *request);

int parse_http_request(const char *buffer, HttpRequest *request);
#endif
