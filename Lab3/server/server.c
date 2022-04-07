#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <errno.h>
#define SERVER_PORT 8080
#define MAX_LINE BUFSIZ
#define IP_ADDRESS "127.0.0.1"

void error_handler(char *error_msg, int sock_fd);

void clear_memory(void *buf);

struct sockaddr_in initialize(struct sockaddr_in sin);

int socket_creation(struct sockaddr_in sin);

void recv_from_client(char *buf, int sock_fd, struct sockaddr_storage clientaddr);

void send_file_to_client(char *buf, int sock_fd, struct sockaddr_storage clientaddr);

int main()
{
    struct sockaddr_storage clientaddr;
    struct sockaddr_in sin;
    char buf[MAX_LINE];

    sin = initialize(sin);
    int sock_fd = socket_creation(sin);

    while (1)
    {
        recv_from_client(buf, sock_fd, clientaddr);
    }
    return 0;
}

void error_handler(char *error_msg, int sock_fd)
{
    perror(error_msg);
    close(sock_fd);
    exit(EXIT_FAILURE);
}

void clear_memory(void *buf)
{
    bzero(buf, sizeof(buf));
}

struct sockaddr_in initialize(struct sockaddr_in sin)
{
    clear_memory(&sin);

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr("127.0.0.1");
    sin.sin_port = htons(SERVER_PORT);

    return sin;
}

int socket_creation(struct sockaddr_in sin)
{
    int s = socket(PF_INET, SOCK_DGRAM, 0);
    if (s < 0)
    {
        error_handler("Failed to create socket!", s);
    }
    puts("Socket created");

    struct timeval tv;
    tv.tv_sec = 1; // Timeout for 1 sec.
    tv.tv_usec = 0;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));

    int b = bind(s, (struct sockaddr *)&sin, sizeof(sin));
    if (b < 0)
    {
        error_handler("Failed to bind socket!", s);
    }
    puts("Bind done");

    return s;
}

void recv_from_client(char *buf, int sock_fd, struct sockaddr_storage clientaddr)
{
    int len;
    socklen_t clientaddr_len = sizeof(clientaddr);

    while ((len = recvfrom(sock_fd, buf, MAX_LINE, 0, (struct sockaddr *)&clientaddr, (socklen_t *)&clientaddr_len)) > 0)
    {
        if (len > 0 && strcmp(buf, "GET") == 0)
        {
            printf("%s\n", buf);
            clear_memory(buf);
            send_file_to_client(buf, sock_fd, clientaddr);
            clear_memory(buf);
        }
        while (1)
        {
            len = recvfrom(sock_fd, buf, MAX_LINE, MSG_DONTWAIT, (struct sockaddr *)&clientaddr, (socklen_t *)&clientaddr_len);

            if (len > 0 && strcmp(buf, "ACK") == 0)
            {
                printf("%s\n", buf);
                clear_memory(buf);
                break;
            }
            else if (errno == EAGAIN)
            {
                printf("ACK Not Recieved\n");
                bzero(buf, MAX_LINE);
                strcpy(buf, "Hi\0");

                if (sendto(sock_fd, buf, sizeof(buf), 0, (const struct sockaddr *)&clientaddr, clientaddr_len) < 0)
                {
                    error_handler("Failed to send through socket!", sock_fd);
                }
            }
        }
    }
}

void send_file_to_client(char *buf, int sock_fd, struct sockaddr_storage clientaddr)
{
    int loop_count = 0, bytes_read = 0, total_bytes = 0;
    socklen_t clientaddr_len = sizeof(clientaddr);
    strcpy(buf, "Hi\0");

    if (sendto(sock_fd, buf, sizeof(buf), 0, (const struct sockaddr *)&clientaddr, clientaddr_len) < 0)
    {
        error_handler("Failed to send through socket!", sock_fd);
    }
    clear_memory(buf);
}