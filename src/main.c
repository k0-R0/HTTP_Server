#include <arpa/inet.h>
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
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("failed to create server socket\n");
        return 1;
    }
    // create a sockaddr_in to store server IP and host number
    struct sockaddr_in server_addr, client_addr;
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

    printf("Waiting for client to connect...\n");
    // accept incoming connections
    int client_fd =
        accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd == -1) {
        perror("accept failed\n");
        return 1;
    }
    printf("client connected\nIP -> %s\nPort -> %hu",
           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    close(server_fd);
}
