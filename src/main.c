#include "status_messages.h"
#include "utils.h"
#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 4221
#define SERVER_ADDRESS "127.0.0.1"

int main() {
    // create a TCP socket for the server
    int server_fd, client_fd;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("failed to create server socket\n");
        return 1;
    }
    // set socket to reuse address so that it doesn't wait for socket creation
    // timeouts while binding
    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse,
                   sizeof(reuse)) == -1) {
        perror("Failed to reuse socket\n");
        return 1;
    }
    //  create a sockaddr_in to store server IP and host number
    struct sockaddr_in server_addr, client_addr;
    // set server members to null / 0
    memset(&server_addr, 0, sizeof(server_addr));
    socklen_t client_len = sizeof(client_addr);

    if (inet_pton(AF_INET, SERVER_ADDRESS, &server_addr.sin_addr) == -1) {
        perror("Failed to convert host order to network order\n");
        return 1;
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    //  bind this to a port to
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
        -1) {
        perror("Failed to bind\n");
        return 1;
    }
    // listen incoming connections
    // keep a queue of 5 connections
    int connection_backlog = 5;
    if (listen(server_fd, connection_backlog) == -1) {
        perror("Failed to connect\n");
        return 1;
    }
    while (1) {

        printf("Waiting for client to connect...\n");
        // accept incoming connections
        client_fd =
            accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1) {
            perror("accept failed\n");
            break;
        }
        printf("client connected\nIP -> %s\nPort -> %hu\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        // read client request URL
        char client_req_buffer[2048];
        if (get_client_request(client_fd, client_req_buffer,
                               sizeof(client_req_buffer))) {
            close(client_fd);
            continue;
        }
        //  send HTTP status line to client
        // else send 404
        char page_buffer[200];
        get_page(client_req_buffer, page_buffer);
        if (strcmp(page_buffer, "/") == 0) {
            if (send_success(client_fd)) {
                break;
            }
        } else {
            // if valid send 200
            if (send_failure(client_fd)) {
                break;
            }
        }
        close(client_fd);
    }
    close(server_fd);
}
