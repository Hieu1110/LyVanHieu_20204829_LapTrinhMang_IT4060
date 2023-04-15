#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    // Kiểm tra số lượng tham số đầu vào
    if (argc != 3) {
        printf("Sử dụng: %s [port] [log file name]\n", argv[0]);
        return 1;
    }

    // Khởi tạo socket
    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock < 0) {
        perror("Không thể tạo socket");
        return 1;
    }

    // Thiết lập địa chỉ và cổng của server
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));

    // Gán địa chỉ và cổng cho socket
    if (bind(listen_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Không thể gán địa chỉ và cổng cho socket");
        return 1;
    }

    // Bắt đầu lắng nghe kết nối từ client
    if (listen(listen_sock, 5) < 0) {
        perror("Lỗi trong quá trình lắng nghe kết nối");
        return 1;
    }

    // Mở file log
    FILE *log_file = fopen(argv[2], "a");
    if (log_file == NULL) {
        perror("Không thể mở file log");
        return 1;
    }

    // Vòng lặp chính để nhận kết nối từ client
    while (1) {
        // Chấp nhận kết nối từ client
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_sock = accept(listen_sock, (struct sockaddr*)&client_addr, &client_len);
        if (client_sock < 0) {
            perror("Lỗi trong quá trình chấp nhận kết nối");
            return 1;
        }

        // Nhận dữ liệu từ client
        char buffer[BUFFER_SIZE];
        int bytes_received = recv(client_sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received < 0) {
            perror("Lỗi trong quá trình nhận dữ liệu từ client");
            return 1;
        }

        // Thêm ký tự kết thúc chuỗi
        buffer[bytes_received] = '\0';

        // Lấy địa chỉ IP của client và thời gian hiện tại
        char *client_ip = inet_ntoa(client_addr.sin_addr);
        time_t raw_time;
        time(&raw_time);
        struct tm *timeinfo = localtime(&raw_time);


        // Ghi dữ liệu vào file log
        time_t current_time;
        time(&current_time);
        struct tm* time_info = localtime(&current_time);
        char time_str[80];
        strftime(time_str, 80, "%Y-%m-%d %H:%M:%S", time_info);

        fprintf(log_file, "%s %s\n", inet_ntoa(client_addr.sin_addr), time_str);
        fprintf(log_file, "%s\n", buffer);

        // In dữ liệu ra màn hình
        printf("%s %s %s\n", inet_ntoa(client_addr.sin_addr), time_str, buffer);

        // Đóng socket
        close(client_sock);
    }

    // Đóng file log và socket chính
    fclose(log_file);
    close(listen_sock);
    return 0;
}
