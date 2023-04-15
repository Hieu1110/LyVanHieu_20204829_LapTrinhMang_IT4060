#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_MSG_LENGTH 1024

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server;
    char message[MAX_MSG_LENGTH], server_reply[MAX_MSG_LENGTH];
    
    // Kiểm tra định dạng tham số đầu vào
    if (argc != 3) {
        printf("Sử dụng: %s <địa chỉ IP> <cổng>\n", argv[0]);
        return 1;
    }
    
    // Tạo socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Không thể tạo socket\n");
        return 1;
    }
    
    // Cấu hình địa chỉ server
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));
    
    // Kết nối tới server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Không thể kết nối tới server\n");
        return 1;
    }
    
    // Nhập dữ liệu từ bàn phím và gửi tới server
    printf("Nhập dữ liệu: ");
    fgets(message, MAX_MSG_LENGTH, stdin);
    if (send(sock, message, strlen(message), 0) < 0) {
        printf("Gửi dữ liệu thất bại\n");
        return 1;
    }
    
    // Nhận phản hồi từ server
    if (recv(sock, server_reply, MAX_MSG_LENGTH, 0) < 0) {
        printf("Nhận phản hồi thất bại\n");
        return 1;
    }
    
    // In phản hồi từ server
    printf("Phản hồi từ server: %s\n", server_reply);
    
    close(sock);
    return 0;
}
