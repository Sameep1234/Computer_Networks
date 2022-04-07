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
#define SERVER_PORT 8080
#define MAX_LINE BUFSIZ
#define IP_ADDRESS "127.0.0.1"

void error_handler(char *error_msg, int sock_fd);

char *command_line(int argc, char *argv[], char *host);

void clear_memory(void *buf);

struct sockaddr_in initialize(struct sockaddr_in sin, struct hostent *hp, char *host);

int socket_creation(struct sockaddr_in sin);

void send_to_server(char *new_buf, int sock_fd, struct sockaddr_in sin);

void recv_file_from_server(char *buf, int sock_fd, struct sockaddr_storage servaddr, socklen_t servaddr_len, struct sockaddr_in sin);
int main(int argc, char *argv[])
{
    /* Defining required variables */
    struct hostent *hp;
    struct sockaddr_in sin;
    struct sockaddr_storage servaddr;
    // char *fileName = "sample.mp4";
    socklen_t servaddr_len = sizeof(struct sockaddr);
    char buf[MAX_LINE];
    char new_buf[MAX_LINE];
    char *host;
    FILE *fp;

    host = command_line(argc, argv, host);

    sin = initialize(sin, hp, host);

    int sock_fd = socket_creation(sin);

    send_to_server(buf, sock_fd, sin);

    recv_file_from_server(buf, sock_fd, servaddr, servaddr_len, sin);

    return 0;
}

void error_handler(char *error_msg, int sock_fd)
{
    perror(error_msg);
    close(sock_fd);
    exit(EXIT_FAILURE);
}

char *command_line(int argc, char *argv[], char *host)
{
    if (argc == 2)
    {
        host = argv[1];
    }
    else
    {
        perror("Invalid Argument Count!");
        exit(EXIT_FAILURE);
    }
    return host;
}

void clear_memory(void *buf)
{
    bzero(buf, sizeof(buf));
}

struct sockaddr_in initialize(struct sockaddr_in sin, struct hostent *hp, char *host)
{
    printf("%s\n", host);
    hp = gethostbyname(host);

    if (!hp)
    {
        perror("Host entry Failed!");
        exit(EXIT_FAILURE);
    }
    clear_memory(&sin);

    sin.sin_family = AF_INET;
    sin.sin_port = htons(SERVER_PORT);

    bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
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
    return s;
}

void send_to_server(char *new_buf, int sock_fd, struct sockaddr_in sin)
{
    strcpy(new_buf, "GET\0");
    sendto(sock_fd, new_buf, MAX_LINE - 1, 0, (const struct sockaddr *)&sin, (socklen_t)sizeof(struct sockaddr_in));
}

void recv_file_from_server(char *buf, int sock_fd, struct sockaddr_storage servaddr, socklen_t servaddr_len, struct sockaddr_in sin)
{
    // fp = fopen(fileName, "wb");
    //  if (NULL == fp)
    //  {
    //      error_handler("Failed to open the file!", sock_fd);
    //  }
    int total_bytes = 0, loop_count = 0, bytes = 0;
    
    bytes = recvfrom(sock_fd, buf, sizeof(buf), 0, (struct sockaddr *)&servaddr, &servaddr_len);
    
        // total_bytes += bytes;
        // loop_count++;
        // printf("Bytes recieved: %d\n", bytes);
        // if (strcmp(buf, "BYE") == 0)
        // {
        //     printf("EOF Recieved!\n");
        //     break;
        // }
        //fwrite(buf, 1, MAX_LINE - 1, fp);
        printf("Block 1 : %s\n",buf);
        clear_memory(buf);
       
        strcpy(buf,"ACK\0");
        sendto(sock_fd, buf, MAX_LINE - 1, 0, (const struct sockaddr *)&sin, (socklen_t)sizeof(struct sockaddr_in));

    
    // printf("Total Bytes Read: %d\n", total_bytes);
    // printf("Loop Count: %d\n", loop_count);
    // fclose(fp);
}