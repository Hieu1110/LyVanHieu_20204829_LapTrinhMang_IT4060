#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int main(int argc, char **argv) {
    if (argc < 4) {
        printf("Cách sử dụng: %s <cổng> <tệp chào> <tệp tin nhắn>\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);
    char *greeting_file = argv[2];
    char *client_file = argv[3];

    // Mở file chào
    FILE *greeting_fp = fopen(greeting_file, "r");
    if (greeting_fp == NULL) {
        printf("Lỗi: không thể mở tệp lời chào.\n");
        exit(1);
    }

    // Đọc lời chào từ file
    char greeting_buffer[BUFFER_SIZE];
    fgets(greeting_buffer, BUFFER_SIZE, greeting_fp);

    // Tạo socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        printf("Lỗi: không thể tạo socket.\n");
        exit(1);
    }

    // Đặt socket ở chế độ không chặn
    fcntl(server_fd, F_SETFL, O_NONBLOCK);

    // Liên kết socket với host
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        printf("Lỗi: không thể liên kết socket với host.\n");
        exit(1);
    }

    // Lắng nghe các kết nối đến
    if (listen(server_fd, MAX_CLIENTS) == -1) {
        printf("Lỗi: không thể lắng nghe các kết nối đến.\n");
        exit(1);
    }

    // Chấp nhận các kết nối đến và xử lý chúng
    while (1) {
        // Chấp nhận kết nối
        struct sockaddr_in client_address;
        socklen_t client_address_size = sizeof(client_address);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_address_size);

        if (client_fd != -1) {
            printf("Chấp nhận kết nối mới.\n");

            // Viết lời chào tới client
            send(client_fd, greeting_buffer, strlen(greeting_buffer), 0);

            // Mở file tin nhắn client
            FILE *client_fp = fopen(client_file, "a");
            if (client_fp == NULL) {
                printf("Lỗi: không thể mở tệp tin nhắn của client.\n");
                exit(1);
            }

            // Nhận dữ liệu từ client và ghi vào file
            char buffer[BUFFER_SIZE];
            int bytes_received;
            while ((bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0)) > 0) {
                fwrite(buffer, sizeof(char), bytes_received, client_fp);
            }

            // Đóng tệp tin nhắn khách hàng và ổ cắm máy khách
            fclose(client_fp);
            close(client_fd);

            printf("Đã lưu tin nhắn của client.\n");
        }
    }
}
   
