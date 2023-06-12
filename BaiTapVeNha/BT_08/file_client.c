#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

void receive_file_list(int server_socket) {
    char file_list[BUFFER_SIZE];
    memset(file_list, 0, sizeof(file_list));
    
    // Nhận danh sách file từ server
    ssize_t read_bytes = recv(server_socket, file_list, sizeof(file_list) - 1, 0);
    
    if (read_bytes > 0) {
        file_list[read_bytes] = '\0';
        
        if (strncmp(file_list, "ERROR", 5) == 0) {
            printf("Server response: %s", file_list);
        } else {
            // In danh sách file
            printf("Server response:\n%s\n", file_list);
        }
    } else {
        perror("Error: receive failed");
    }
    
    // Đóng kết nối
    close(server_socket);
}

void receive_file(int server_socket, char *file_name) {
    char response[BUFFER_SIZE];
    
    // Gửi tên file cho server
    send(server_socket, file_name, strlen(file_name), 0);
    
    // Nhận phản hồi từ server
    ssize_t read_bytes = recv(server_socket, response, sizeof(response) - 1, 0);
    
    if (read_bytes > 0) {
        response[read_bytes] = '\0';
        
        if (strncmp(response, "ERROR", 5) == 0) {
            printf("Server response: %s\n", response);
        } else {
            // Nhận kích thước file
            long file_size;
            sscanf(response, "OK %ld", &file_size);
            
            // Nhận nội dung file
            char file_content[BUFFER_SIZE];
            FILE *fp = fopen(file_name, "wb");
            
            if (fp != NULL) {
                long remaining_bytes = file_size;
                ssize_t write_bytes;
                
                while (remaining_bytes > 0) {
                    ssize_t read_bytes = recv(server_socket, file_content, sizeof(file_content), 0);
                    
                    if (read_bytes > 0) {
                        write_bytes = fwrite(file_content, 1, read_bytes, fp);
                        
                        if (write_bytes < read_bytes) {
                            perror("Error: write failed");
                            break;
                        }
                        
                        remaining_bytes -= write_bytes;
                    } else {
                        perror("Error: receive failed");
                        break;
                    }
                }
                
                printf("File \"%s\" downloaded successfully.\n", file_name);
                fclose(fp);
            } else {
                perror("Error: file creation failed");
            }
        }
    } else {
        perror("Error: receive failed");
    }
    
    // Đóng kết nối
    close(server_socket);
}

int main() {
    int server_socket;
    struct sockaddr_in server_address;
    
    // Tạo socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    if (server_socket == -1) {
        perror("Error: socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Cài đặt địa chỉ server
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8888);
    
    // Kết nối đến server
    if (inet_pton(AF_INET, "127.0.0.1", &(server_address.sin_addr)) <= 0) {
        perror("Error: invalid server address");
        exit(EXIT_FAILURE);
    }
    
    if (connect(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Error: connection failed");
        exit(EXIT_FAILURE);
    }
    
    // Nhận danh sách file từ server
    receive_file_list(server_socket);
    
    // Nhập tên file cần tải về
    char file_name[100];
    printf("Enter file name to download: ");
    fgets(file_name, sizeof(file_name), stdin);
    
    // Xóa ký tự xuống dòng cuối cùng
    file_name[strcspn(file_name, "\n")] = '\0';
    
    // Tải về file từ server
    receive_file(server_socket, file_name);
    
    return 0;
}
