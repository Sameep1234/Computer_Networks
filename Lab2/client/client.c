/* AU1940177 Kairavi Shah */
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define SERVER_PORT 8080
#define MAX_LINE BUFSIZ
#define IP_ADDRESS "127.0.0.1"

void error_handler(char *error_msg)
{
    perror(error_msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    /* Defining required variables */
    struct hostent *hp;
    struct sockaddr_in sin;
    struct sockaddr_storage servaddr;
    char *fileName = "sample.mp4";
    socklen_t servaddr_len = sizeof(struct sockaddr);
    char buf[MAX_LINE];
    int s, len, c, r;
    char *host;
    FILE *fp;
    int bytes = 0;
    char new_buf[MAX_LINE];

    if (argc == 2)
    {
        host = argv[1];
    }
    else
    {
        error_handler("Argument Count Invalid!");
    }
    /* translate host name into peer’s IP address */
    /* gethostbyname() returns a pointer to a hostent struct or NULL.*/
    hp = gethostbyname(host);
    if (!hp)
    {
        error_handler("Host entry Failed!");
    }

    /*Initialize sockaddr struc to memory byte 0*/
    bzero((char *)&sin, sizeof(sin));
    /*Prepare the sockaddr_in structure*/
    sin.sin_family = AF_INET;
    sin.sin_port = htons(SERVER_PORT);
    /*Copy the address of host in hostent to sockaddr*/
    bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);

    /*Creating a socket*/
    s = socket(PF_INET, SOCK_DGRAM, 0);
    if (s < 0)
    {
        error_handler("Socket creation failure");
    }
    puts("Socket created");
    strcpy(new_buf, "GET\0");
    sendto(s, new_buf, MAX_LINE - 1, 0, (const struct sockaddr *)&sin, (socklen_t)sizeof(struct sockaddr_in));
    fp = fopen(fileName, "wb");
    if (NULL == fp)
    {
        error_handler("Opening file failed!");
    }
    int total_bytes = 0, loop_count = 0;
    while (bytes = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&servaddr, (socklen_t *)&servaddr_len))
    {
        total_bytes += bytes;
        loop_count++;
        printf("Bytes recieved: %d\n", bytes);
        if (strcmp(buf, "BYE") == 0)
        {
            printf("EOF Recieved!\n");
            break;
        }
        fwrite(buf, 1, bytes, fp);
        // printf("Total Bytes Read: %d\n", total_bytes);
        bzero(buf, MAX_LINE);
    }
    printf("Total Bytes Read: %d\n", total_bytes);
    printf("Loop Count: %d\n", loop_count);
    fclose(fp);
    return 0;
}