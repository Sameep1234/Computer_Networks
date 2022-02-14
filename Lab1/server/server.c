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
#define MAX_PENDING 5
#define MAX_LINE 256
#define SERVER_IP "127.0.0.1"

void handle_error(char *error_msg)
{
    perror(error_msg);
    exit(EXIT_FAILURE);
}

int main()
{
    struct sockaddr_in sin;
    char buf[MAX_LINE]; // Buffer
    int sockfd, new_sockfd; // Sockets
    int len; // For storing the length of the msg
    bzero((char *) &sin, sizeof(sin)); // Initialize structure

    /* Modify required variables */
    sin.sin_family = AF_INET; // Address Family, Internet
    sin.sin_addr.s_addr = inet_addr(SERVER_IP); // Ip Address in network format
    sin.sin_port = htons(SERVER_PORT); // Port in network format

    /* Create a socket */
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) // Check for error
    {
        handle_error("Socket Failed");
    }

    /* Bind the socket with particular port and address */
    if(bind(sockfd, (struct sockaddr_in *) &sin, sizeof(sin)) < 0)
    {
        handle_error("Bind Failed");
    }

    /* Mark socket as passive listening using listen() */
    int error_check = listen(sockfd, MAX_PENDING);
    if(error_check < 0) // Check for error
    {
        handle_error("Listen Failed");
    }

    /* Accept the incoming requests */
    while(1)
    {
        new_sockfd = accept(sockfd, (struct sockaddr_in *) &sin, &len);
        if(new_sockfd < 0) // Error Check
        {
            handle_error("Accept Failed");
        }

        /* Send "Hello" from server */
        char *initial_greetings = "Hello";
        if(send(new_sockfd, initial_greetings, sizeof(buf), 0) < 0)
        {
            handle_error("Send Failed");
        }

        /* Recieve request from client */
        while(len = recv(new_sockfd, buf, sizeof(buf), 0))
        {
            if(buf[0] == 'B' && buf[1] == 'y', buf[2] == 'e') // Terminate the socket if client says bye.
            {
                printf("Bye: Closing Connection\n");
                close(new_sockfd);
                break;
            }
            else
            {
                fputs(buf, stdout);
                char *temp = buf;
                printf("%d %ld\n", len, strlen(temp));
            }
        }
    }
    return 0;
}