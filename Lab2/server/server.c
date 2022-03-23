#include "../header.h"

void error_handler(char *error_msg, int sock_fd);

void clear_memory(void *buf);

struct sockaddr_in initialize(struct sockaddr_in sin);

int socket_creation(struct sockaddr_in sin);

void recv_from_client(FILE *fp, char *buf, char *fileName, int sock_fd, struct sockaddr_storage clientaddr);

void send_file_to_client(FILE *fp, char *buf, int sock_fd, struct sockaddr_storage clientaddr);

void send_eof(char *buf, int sock_fd, struct sockaddr_storage clientaddr);

int main()
{
    /* Defining required variables */
    struct sockaddr_storage clientaddr;
    char *fileName = "sample.mp4";
    struct sockaddr_in sin;
    char buf[MAX_LINE];
    int s, new_s, b, len;
    FILE *fp;

    sin = initialize(sin);
    int sock_fd = socket_creation(sin);

    while (1)
    {
        recv_from_client(fp, buf, fileName, sock_fd, clientaddr);
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

    int b = bind(s, (struct sockaddr *)&sin, sizeof(sin));
    if (b < 0)
    {
        error_handler("Failed to bind socket!", s);
    }
    puts("Bind done");

    return s;
}

void recv_from_client(FILE *fp, char *buf, char *fileName, int sock_fd, struct sockaddr_storage clientaddr)
{
    int len;
    socklen_t clientaddr_len = sizeof(clientaddr);
    while ((len = recvfrom(sock_fd, buf, MAX_LINE, 0, (struct sockaddr *)&clientaddr, (socklen_t *)&clientaddr_len)) > 0)
    {
        if (strcmp(buf, "GET") == 0)
        {
            printf("%s\n", buf);
            clear_memory(buf);

            fp = fopen(fileName, "rb");
            if (fp == NULL)
            {
                error_handler("Failed to open file!", sock_fd);
            }

            send_file_to_client(fp, buf, sock_fd, clientaddr);
        }
    }
}

void send_file_to_client(FILE *fp, char *buf, int sock_fd, struct sockaddr_storage clientaddr)
{
    int loop_count = 0, bytes_read = 0, total_bytes = 0;
    socklen_t clientaddr_len = sizeof(clientaddr);

    while (!feof(fp))
    {
        struct timespec t;
        t.tv_nsec = 10000000L;
        nanosleep((const struct timespec *)&t, NULL);

        loop_count++;
        bytes_read = fread(buf, 1, MAX_LINE - 1, fp);
        buf[MAX_LINE - 1] = '\0';

        if (ferror(fp) != 0)
        {
            error_handler("Failed to read file!", sock_fd);
        }
        printf("Bytes Send: %d\n", bytes_read);
        
        total_bytes += bytes_read;
        if (sendto(sock_fd, buf, sizeof(buf), 0, (const struct sockaddr *)&clientaddr, clientaddr_len) < 0)
        {
            error_handler("Failed to send through socket!", sock_fd);
        }
        clear_memory(buf);
    }

    printf("Total Bytes sent: %d\n", total_bytes);
    printf("Loop Count: %d\n", loop_count);

    fclose(fp);

    send_eof(buf, sock_fd, clientaddr);
}


void send_eof(char *buf, int sock_fd, struct sockaddr_storage clientaddr)
{
    socklen_t clientaddr_len = sizeof(clientaddr);

    printf("Sending Bye!\n");
    strcpy(buf, "BYE\0");
    if (sendto(sock_fd, buf, MAX_LINE - 1, 0, (const struct sockaddr *)&clientaddr, clientaddr_len) < 0)
    {
        error_handler("Failed to send \"BYE\"!", sock_fd);
    }
}