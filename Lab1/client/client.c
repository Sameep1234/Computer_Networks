/*
    Name: Sameep Vani
    Roll Number: AU1940049
    Email: sameep.v@ahduni.edu.in
*/

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
#define MAX_LINE 1024 /* Buffer size of 8KB */

/* Handle Error Function */
void handle_error(char *error_msg)
{
    perror(error_msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    FILE *fd;
    struct hostent *hp;     // Create hostent Structure
    int sockfd;             // Socket file descriptor
    char buf[MAX_LINE];     // Create buffer
    char buf_2[MAX_LINE];   // Create buffer
    struct sockaddr_in sin; // Structure for sockaddr_in
    char *host;             // Host

    if (argc == 2)
    {
        host = argv[1]; // Store host name
    }
    else
    {
        /* Error function */
        handle_error("Argument Count Invalid!");
    }

    hp = gethostbyname(host); // Invoke gethostbyname()
    if (!hp)                  // Check Error
    {
        handle_error("Hostentry Failed!");
    }

    bzero((void *)&sin, sizeof(sin));                       // Initialize Memory
    bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length); // Copy address from hostent to sin_addr
    sin.sin_port = htons(SERVER_PORT);                      // Save server port
    sin.sin_family = AF_INET;                               // Address Family, Internet

    sockfd = socket(PF_INET, SOCK_STREAM, 0); // Create socket

    if (sockfd < 0) // Check Error
    {
        handle_error("Socket Creation Failed!");
    }

    /* Send connection to server */
    if (connect(sockfd, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
        /* Check Error */
        handle_error("Connection Failed!");
    }

    /* Recieve the initial greetings from the server */
    recv(sockfd, buf, MAX_LINE, 0);
    fputs(buf, stdout);
    printf("\n");

    /* Write in buffer and send it to server */
    while (fgets(buf_2, MAX_LINE, stdin))
    {
        buf_2[MAX_LINE - 1] = '\0';
        int len = strlen(buf_2) + 1;
        if (send(sockfd, buf_2, len, 0) < 0)
        {
            handle_error("SEND FAILED!!!!");
        }
        char *fileName = buf_2;
        len = strlen(fileName);
        fileName[len - 1] = '\0';

        /* Recieve the response sent from the server */
        len = recv(sockfd, buf, MAX_LINE, 0);
        fputs(buf, stdout);
        printf("\n");

        if (buf[0] == 'O' && buf[1] == 'K')
        {
            bzero(buf, MAX_LINE);
            fd = fopen(fileName, "wb");
            if (fd == NULL)
            {
                handle_error("Opening file failed!");
            }
            int total_bytes = 0;
            while (len = recv(sockfd, buf, sizeof(buf), 0) > 0)
            {
                printf("Bytes Recieved: %ld\n", strlen(buf));
                total_bytes += len;
                // printf("Recived String: %s\n", buf);
                if (buf[0] == 'E' && buf[1] == 'O' && buf[2] == 'F' && strlen(buf) < 5)
                {
                    printf("INSIDE E\n");
                    break;
                }
                fwrite(buf, 1, MAX_LINE - 1, fd);
                bzero(buf, MAX_LINE);
            }
            printf("Total Bytes Recieved: %d\n", total_bytes);
            fclose(fd);
        }
    }

    return 0;
}