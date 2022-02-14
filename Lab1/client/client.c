#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define SERVER_PORT 8089
#define MAX_LINE 256

void handle_error(char *error_msg)
{
    perror(error_msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    struct hostent *hp;
    int sockfd;
    char buf[MAX_LINE];
    struct sockaddr_in sin;
    char *host;

    if(argc == 2)
    {
        host = argv[1];
    }
    else
    {
        handle_error("Argument Count Invalid!");
    }

    hp = gethostbyname(host);
    if (!hp)
    {
        handle_error("Hostentry Failed!");
    }

    bzero((void *) &sin, sizeof(sin));
    bcopy(hp->h_addr, (char *) &sin.sin_addr, hp->h_length);
    sin.sin_port = htons(SERVER_PORT);
    sin.sin_family = AF_INET;

    sockfd = socket(PF_INET, SOCK_STREAM, 0);

    if(sockfd < 0)
    {
        handle_error("Socket Creation Failed!");
    }

    if(connect(sockfd, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
        handle_error("Connection Failed!");
    }

    recv(sockfd, buf, sizeof(buf), 0);
    fputs(buf, stdout);
    printf("\n");

     while (fgets(buf, sizeof(buf), stdin))
    {
        buf[MAX_LINE - 1] = '\0';
        int len = strlen(buf) + 1;
        send(sockfd, buf, len, 0);
    }

    return 0;
}