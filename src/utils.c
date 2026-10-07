#include "status_messages.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

int get_client_request(int sock_fd, char *buffer, size_t buffer_len) {
    int bytes_received = recv(sock_fd, buffer, buffer_len - 1, 0);
    if (bytes_received == -1) {
        perror("client HTTP req failed\n");
        return 1;
    }
    buffer[bytes_received] = '\0';
    printf("%s\n", buffer);
    return 0;
}

void get_page(const char *req, char *buffer) {
    char req_line[300];
    strncpy(req_line, req, sizeof(req_line) - 1);
    req_line[sizeof(req_line) - 1] = '\0';
    req_line[strcspn(req_line, "\r\n")] = '\0';
    if (strncmp(req_line, "GET ", 4) == 0) {
        char *token = strtok(req_line + 4, " ");
        if (token != NULL) {
            strcpy(buffer, token);
            return;
        }
    }
    strcpy(buffer, "Invalid req");
}

int send_success(int sock_fd) {
    printf("sending 200\n");
    char *status_buffer;
    status_buffer = STATUS_200;
    if (send(sock_fd, status_buffer, strlen(status_buffer), 0) == -1) {
        perror("Status send failed\n");
        return 1;
    }
    return 0;
}

int send_failure(int sock_fd) {
    printf("sending 404, page not found\n");
    char *status_buffer;
    status_buffer = STATUS_404;
    if (send(sock_fd, status_buffer, strlen(status_buffer), 0) == -1) {
        perror("Status send failed\n");
        return 1;
    }
    return 0;
}
