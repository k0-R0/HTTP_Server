#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
void get_page(const char *req, char *buffer);
int send_success(int sock_fd);
int send_failure(int sock_fd);
int get_client_request(int sock_fd, char *buffer, size_t buffer_len);
#endif
