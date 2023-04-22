#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in receiver_addr;
    char *file_name;
    char buffer[BUF_SIZE];
    ssize_t read_len;
    FILE *fp;
    size_t file_size;
    ssize_t sent_size;

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <Receiver IP> <Receiver port> <File name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    file_name = argv[3];

    // Open the file to be sent
    fp = fopen(file_name, "rb");
    if (fp == NULL) {
        printf("Loi mo tep!");
        exit(EXIT_FAILURE);
    }

    // Calculate the size of the file
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Create UDP socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        printf("Khong tao duoc socket!");
        exit(EXIT_FAILURE);
    }

    // Set receiver address
    memset(&receiver_addr, 0, sizeof(receiver_addr));
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_addr.s_addr = inet_addr(argv[1]);
    receiver_addr.sin_port = htons(atoi(argv[2]));

    // Send file name to receiver
    sent_size = sendto(sock, file_name, strlen(file_name) + 1, 0, (struct sockaddr *)&receiver_addr, sizeof(receiver_addr));
    if (sent_size == -1) {
        printf("Loi gui ten!");
        exit(EXIT_FAILURE);
    }else printf("Gui ten thanh cong!\n");




    // // Send file size to receiver
    // sent_size = sendto(sock, &file_size, sizeof(file_size), 0, (struct sockaddr *)&receiver_addr, sizeof(receiver_addr));
    // if (sent_size == -1) {
    //     printf("Loi gui!");
    //     exit(EXIT_FAILURE);
    // }else printf("Gui size thanh cong!\n");

    // // Send file data to receiver
    // while ((read_len = fread(buffer, 1, BUF_SIZE, fp)) > 0) {
    //     sent_size = sendto(sock, buffer, read_len, 0, (struct sockaddr *)&receiver_addr, sizeof(receiver_addr));
    //     if (sent_size == -1) {
    //         printf("Loi gui data!");
    //         exit(EXIT_FAILURE);
    //     }else printf("Gui data thanh cong!\n");
    // }




    // Nhập dữ liệu từ bàn phím và gửi tới server
    printf("Nhập dữ liệu: ");
    char message[BUF_SIZE];
    fgets(message, BUF_SIZE, stdin);
    if (sendto(sock, message, strlen(message) + 1, 0, (struct sockaddr *)&receiver_addr, sizeof(receiver_addr)) < 0) {
        printf("Gửi dữ liệu thất bại\n");
        return 1;
    }

    // Close the file and socket
    fclose(fp);
    close(sock);

    return 0;
}
