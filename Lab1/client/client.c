#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#define SERVER_PORT 8089
#define MAX_LINE 256

/* Handle Error Function */
void handle_error(char *error_msg)
{
    perror(error_msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int fd;
    struct hostent *hp; // Create hostent Structure
    int sockfd; // Socket file descriptor
    char buf[BUFSIZ]; // Create buffer
    struct sockaddr_in sin; // Structure for sockaddr_in
    char *host; // Host

    if(argc == 2)
    {
        host = argv[1]; // Store host name
    }
    else
    {
        /* Error function */
        handle_error("Argument Count Invalid!");
    }

    hp = gethostbyname(host); // Invoke gethostbyname()
    if (!hp) // Check Error
    {
        handle_error("Hostentry Failed!");
    }

    bzero((void *) &sin, sizeof(sin)); // Initialize Memory
    bcopy(hp->h_addr, (char *) &sin.sin_addr, hp->h_length); // Copy address from hostent to sin_addr
    sin.sin_port = htons(SERVER_PORT); // Save server port
    sin.sin_family = AF_INET; // Address Family, Internet

    sockfd = socket(PF_INET, SOCK_STREAM, 0); // Create socket

    if(sockfd < 0) // Check Error
    {
        handle_error("Socket Creation Failed!");
    }

    /* Send connection to server */
    if(connect(sockfd, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
        /* Check Error */
        handle_error("Connection Failed!");
    }

    /* Recieve the initial greetings from the server */
    recv(sockfd, buf, sizeof(buf), 0);
    fputs(buf, stdout);
    printf("\n");

    /* Write in buffer and send it to server */
    while (fgets(buf, sizeof(buf), stdin))
    {
        buf[MAX_LINE - 1] = '\0';
        int len = strlen(buf) + 1;
        send(sockfd, buf, len, 0);
        char *fileName = buf;

        /* Recieve the response sent from the server */
        len = recv(sockfd, buf, BUFSIZ, 0);
        fputs(buf, stdout);
        printf("\n");

        if(buf[0] == 'O' && buf[1] == 'K')
        {
            len = recv(sockfd, buf, BUFSIZ, 0);
            if(len < 0)
            {
                handle_error("File Recieving Failed!");
            }
            fd = open("sample.txt", O_WRONLY);
            if(fd < 0)
            {
                handle_error("Write File Failed!");
            }
            write(fd, buf, sizeof(buf));
            bzero(buf, BUFSIZ);
            close(fd);
        }
    }

    return 0;
}