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

static const char *get_status(unsigned short status) {
    switch (status) {
    case 200:
        return "OK";
    case 201:
        return "Created";
    case 404:
        return "Not Found";
    default:
        return "Unknown";
    }
}

int send_http_response(int sock_fd, unsigned short status,
                       const char *content_type, const char *response_body,
                       size_t response_len) {
    printf("Sending %hu\n", status);
    const char *status_text = get_status(status);
    char header_buffer[512];
    int header_len;
    if (response_body != NULL && response_len > 0) {
        header_len =
            snprintf(header_buffer, sizeof(header_buffer),
                     "HTTP/1.1 %hu %s\r\nContent-Type: "
                     "%s\r\nContent-Length: %zu\r\n\r\n",
                     status, status_text,
                     content_type ? content_type : "text/plain", response_len);
    } else {
        header_len = snprintf(header_buffer, sizeof(header_buffer),
                              "HTTP/1.1 %hu %s\r\n\r\n", status, status_text);
    }
    if (send(sock_fd, header_buffer, header_len, 0) == -1) {
        perror("Header send failed\n");
        return 1;
    }
    if (response_body != NULL && response_len > 0) {
        if (send(sock_fd, response_body, response_len, 0) == -1) {
            perror("Response body send failed\n");
            return 1;
        }
    }
    return 0;
}
