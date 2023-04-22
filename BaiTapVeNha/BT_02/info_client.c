#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

void error_handling(char *msg);

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    char name[BUF_SIZE], drives[BUF_SIZE], message[BUF_SIZE];
    int drive_count, drive_size;
    char drive_letter, drive_size_str[BUF_SIZE];

    if (argc != 3) {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error");

    printf("Enter the computer name: ");
    fgets(name, BUF_SIZE, stdin);
    name[strlen(name) - 1] = '\0';  // remove newline character

    printf("Enter the number of drives: ");
    scanf("%d", &drive_count);
    while (getchar() != '\n');  // remove remaining characters in stdin

    strcpy(message, name);
    sprintf(message + strlen(message), "\n%d\n", drive_count);

    for (int i = 0; i < drive_count; i++) {
        printf("Enter the drive letter and size (in GB) separated by a space (e.g. C 500): ");
        scanf("%c %d", &drive_letter, &drive_size);
        while (getchar() != '\n');  // remove remaining characters in stdin

        sprintf(drive_size_str, "%d", drive_size);
        strcat(message, &drive_letter);
        strcat(message, " - ");
        strcat(message, drive_size_str);
        strcat(message, "GB\n");
    }

    write(sock, message, strlen(message));
    close(sock);

    return 0;
}

void error_handling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
