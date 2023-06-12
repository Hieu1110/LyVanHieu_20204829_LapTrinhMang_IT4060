#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define FILE_PATH "./files/" // Đường dẫn tới thư mục chứa các file

void send_file_list(int client_socket) {
    char file_list[BUFFER_SIZE];
    memset(file_list, 0, sizeof(file_list));
    FILE *fp;
    
    // Mở thư mục chứa các file
    fp = popen("ls " FILE_PATH, "r");
    
    if (fp == NULL) {
        // Gửi thông báo lỗi nếu không thể mở thư mục
        send(client_socket, "ERROR No files to download\r\n", strlen("ERROR No files to download\r\n"), 0);
    } else {
        // Gửi danh sách các file cho client
        sprintf(file_list, "OK \n");
        char file_name[100];
        int file_count = 0;
        
        // Đọc tên các file và gửi danh sách
        while (fgets(file_name, sizeof(file_name), fp) != NULL) {
            file_count++;
            strcat(file_list, file_name);
        }
        
        if (file_count > 0) {
            strcat(file_list, "\r\n");
            send(client_socket, file_list, strlen(file_list), 0);
        } else {
            // Gửi thông báo lỗi nếu không có file nào trong thư mục
            send(client_socket, "ERROR No files to download\r\n", strlen("ERROR No files to download\r\n"), 0);
        }
        
        // Đóng thư mục
        pclose(fp);
    }
    
    // Đóng kết nối
    close(client_socket);
}

void send_file(int client_socket, char *file_name) {
    char file_path[100];
    sprintf(file_path, "%s%s", FILE_PATH, file_name);
    FILE *fp;
    
    // Mở file
    fp = fopen(file_path, "rb");
    
    if (fp == NULL) {
        // Gửi thông báo lỗi nếu không thể mở file
        send(client_socket, "ERROR File not found\r\n", strlen("ERROR File not found\r\n"), 0);
    } else {
        // Tính kích thước file
        fseek(fp, 0, SEEK_END);
        long file_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        
        // Gửi thông báo OK và kích thước file
        char response[BUFFER_SIZE];
        sprintf(response, "OK %ld\r\n", file_size);
        send(client_socket, response, strlen(response), 0);
        
        // Gửi nội dung file
        char buffer[BUFFER_SIZE];
        size_t read_bytes;
        
        while ((read_bytes = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
            send(client_socket, buffer, read_bytes, 0);
        }
        
        // Đóng file
        fclose(fp);
    }
    
    // Đóng kết nối
    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    int client_address_length = sizeof(client_address);
    
    // Tạo socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    if (server_socket == -1) {
        perror("Error: socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Cài đặt địa chỉ server
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8888);
    
    // Gắn socket với địa chỉ
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Error: bind failed");
        exit(EXIT_FAILURE);
    }
    
    // Lắng nghe kết nối
    if (listen(server_socket, 5) < 0) {
        perror("Error: listen failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Server is listening on port 8888...\n");
    
    while (1) {
        // Chấp nhận kết nối từ client
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t *)&client_address_length);
        
        if (client_socket < 0) {
            perror("Error: accept failed");
            exit(EXIT_FAILURE);
        }
        
        printf("New client connected: %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
        
        // Gửi danh sách file cho client
        send_file_list(client_socket);
    }
    
    // Đóng socket
    close(server_socket);
    
    return 0;
}
