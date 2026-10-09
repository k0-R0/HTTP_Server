#include "status_messages.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

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

int parse_http_request(const char *buffer, HttpRequest *request) {
    request->raw = buffer;
    if (sscanf(buffer, "%15s %255s %15s", request->method, request->path,
               request->version) < 2) {
        return 1;
    }
    return 0;
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

int handle_homepage(int client_fd) {
    char response_buffer[512] = "This is the homepage";
    if (send_http_response(client_fd, 200, NULL, response_buffer,
                           strlen(response_buffer))) {
        return 1;
    }
    return 0;
}

int handle_echo(int client_fd, const char *str) {
    return send_http_response(client_fd, 200, NULL, str, strlen(str));
}

int handle_not_found(int client_fd) {
    // else send 404
    char response_buffer[128] = "Page Not Found";
    return send_http_response(client_fd, 404, NULL, response_buffer,
                              strlen(response_buffer));
}

int get_header_value(const char *raw_req, const char *header_name,
                     char *header_value, int *header_size) {
    char header_prefix[128];
    int prefix_len =
        snprintf(header_prefix, sizeof(header_prefix), "%s: ", header_name);

    char *header = strstr(raw_req, header_prefix);
    if (header == NULL)
        return 1;

    header += prefix_len;
    int header_len = strcspn(header, "\r\n");
    if (header_len >= *header_size) {
        header_len = *header_size - 1;
    }

    strncpy(header_value, header, header_len);
    header_value[header_len] = '\0';
    *header_size = header_len;
    return 0;
}

int handle_user_agent(int client_fd, const HttpRequest *request) {
    char user_agent[512];
    int header_len = sizeof(user_agent);
    if (get_header_value(request->raw, "User-Agent", user_agent, &header_len) ==
        0)
        return send_http_response(client_fd, 200, NULL, user_agent, header_len);

    strcpy(user_agent, "User-Agent Not Found");
    header_len = strlen(user_agent);
    return send_http_response(client_fd, 404, NULL, user_agent, header_len);
}

int route_request(int client_fd, const HttpRequest *request) {
    // if valid send 200
    if (strcmp(request->method, "GET") == 0) {
        if (strcmp(request->path, "/") == 0)
            return handle_homepage(client_fd);
        else if (strncmp(request->path, "/echo/", 6) == 0)
            return handle_echo(client_fd, request->path + 6);
        else if (strcmp(request->path, "/user-agent") == 0)
            return handle_user_agent(client_fd, request);
    }
    return handle_not_found(client_fd);
}
