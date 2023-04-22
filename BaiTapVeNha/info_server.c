#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_SIZE 1024

int main(int argc, char *argv[]) {
    // Kiểm tra số lượng tham số đầu vào
    if (argc != 2) {
        printf("Sử dụng: %s [port] \n", argv[0]);
        return 1;
    }


    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[MAX_SIZE];
    char computername[MAX_SIZE];
    char *drive_info[MAX_SIZE];
    char *token;
    int drive_count, i;

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(argv[1]));

    // Bind socket to address
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Failed to bind socket to address");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 1) == -1) {
        perror("Failed to listen for connections");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for connections...\n");

    while (1) {
        // Accept connection
        socklen_t client_addr_len = sizeof(client_addr);
        if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len)) == -1) {
            perror("Failed to accept connection");
            continue;
        }

        printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Receive data from client
        if (recv(client_fd, buffer, MAX_SIZE, 0) == -1) {
            perror("Failed to receive data from client");
            close(client_fd);
            continue;
        }

        // Parse client data
        token = strtok(buffer, ";");
        strcpy(computername, token);
        printf("Computer name: %s\n", computername);

        token = strtok(NULL, ";");
        drive_count = atoi(token);
        printf("Number of drives: %d\n", drive_count);

        for (i = 0; i < drive_count; i++) {
            token = strtok(NULL, ";");
            drive_info[i] = token;
            printf("%s\n", drive_info[i]);
        }

        printf("\n");

        // Close connection
        close(client_fd);
    }

    return 0;
}
