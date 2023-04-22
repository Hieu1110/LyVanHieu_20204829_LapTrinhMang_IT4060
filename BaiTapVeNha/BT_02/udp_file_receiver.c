#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {


    int sock;
    struct sockaddr_in receiver_addr;
    char *file_name;
    char buffer[BUF_SIZE];
    ssize_t read_len, total_received = 0;
    FILE *fp;
    size_t file_size, received_size = 0;


    if (argc != 2) {
        printf("Sử dụng: ./udp_file_receiver <port>\n");
        exit(1);
    }

    int port = atoi(argv[1]);

    // Tạo socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Không thể tạo socket");
        exit(1);
    }

    // Thiết lập địa chỉ và cổng chờ
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    // Gắn socket với địa chỉ và cổng
    if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("Không thể gắn socket với địa chỉ");
        exit(1);
    }

    printf("UDP File Receiver đang chạy...\n");

    struct sockaddr_in sender_addr;
    socklen_t sender_len = sizeof(sender_addr);
    char buf[BUF_SIZE];
    bzero(buf, BUF_SIZE);

    // Nhận dữ liệu từ udp_file_sender và ghi vào file
    int total_bytes = 0;
    FILE *file;
    while (1) {
        int recv_bytes = recvfrom(sockfd, buf, BUF_SIZE, 0, (struct sockaddr *) &sender_addr, &sender_len);
        if (recv_bytes < 0) {
            perror("Lỗi khi nhận dữ liệu");
            exit(1);
        }
        if (strcmp(buf, "END") == 0) {
            break;
        }

        printf("Dia chi: %s, ten tep: %s\n", inet_ntoa(sender_addr.sin_addr), buf);

        if (total_bytes == 0) {
            // Lần đầu nhận dữ liệu - tạo file và mở để ghi
            file = fopen(buf, "wb");
            if (file == NULL) {
                perror("Không thể tạo file");
                exit(1);
            }
        } else {
            // Ghi dữ liệu vào file
            printf("Tao file thanh cong!");
            fwrite(buf, 1, recv_bytes, file);
        }

        // Nhận dữ liệu từ client và ghi vào file
            char buffer2[BUF_SIZE];
            int bytes_received;
            while ((bytes_received = recvfrom(sockfd, buffer2, BUF_SIZE, 0, (struct sockaddr *) &sender_addr, &sender_len)) > 0) {
                fwrite(buffer2, sizeof(char), bytes_received, file);
            }
            printf("Đã lưu tin nhắn của client.\n");


        total_bytes += recv_bytes;
    }

    // Đóng file và socket
    fclose(file);
    close(sockfd);

    printf("Đã nhận được %d byte dữ liệu\n", total_bytes);

    return 0;
}
