#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    // Kiểm tra số lượng tham số đầu vào
    if (argc != 3) {
        printf("Usage: %s <server_ip> <port>\n", argv[0]);
        return 1;
    }

    // Tạo socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Could not create socket");
        return 1;
    }

    // Địa chỉ IP và cổng kết nối tới server
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(argv[1]);
    server_address.sin_port = htons(atoi(argv[2]));

    // Kết nối tới server
    if (connect(client_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == -1) {
        perror("Could not connect to server");
        close(client_socket);
        return 1;
    }

    // Mở file văn bản
    char file_path[BUFFER_SIZE];
    printf("Enter file path: ");
    fgets(file_path, BUFFER_SIZE, stdin);
    file_path[strcspn(file_path, "\r\n")] = 0;  // Xóa ký tự xuống dòng
    FILE* file = fopen(file_path, "r");
    if (file == NULL) {
        printf("Could not open file %s\n", file_path);
        close(client_socket);
        return 1;
    }

    // Đọc nội dung file và gửi lên server
    char greeting_buffer[BUFFER_SIZE];
    fgets(greeting_buffer, BUFFER_SIZE, file);


    if (send(client_socket, greeting_buffer, strlen(greeting_buffer), 0) == -1) {
        perror("Could not send data to server");
        fclose(file);
        close(client_socket);
        return 1;
    }
    printf("Sent data to server.\n");

    // Đóng file và socket
    fclose(file);
    close(client_socket);

    return 0;
}
