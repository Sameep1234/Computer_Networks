#include "../header.h"

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
    printf("Connection successful\n");

    /* Recieve the station list from the server */
    while (recv(sockfd, buf, MAX_LINE, 0) > 0)
    {
        fputs(buf, stdout);
        char temp;
        scanf("%c", &temp);
        printf("\nRead Char: %c\n", temp);
        
        bzero(buf, MAX_LINE);
        strcpy(buf, &temp);
        printf("Read Buf: %s\n", buf);

        if(send(sockfd, buf, MAX_LINE, 0) < 0)
        {
            handle_error("Sending Station Number Failed!");
        }
        // printf("\nValue in buf is: %s\n", buf);
    }
    return 0;
}