#include "../header.h"

void handle_error(char *error_msg, int socket_fd)
{
    close(socket_fd);
    perror(error_msg);
    exit(EXIT_FAILURE);
}

int main()
{
    struct sockaddr_in sin;
    char buf[MAX_LINE]; // Buffer
    printf("%ld\n", sizeof(buf));
    int sockfd, new_sockfd;           // Sockets
    int len;                          // For storing the length of the msg
    bzero((char *)&sin, sizeof(sin)); // Initialize structure

    /* Modify required variables */
    sin.sin_family = AF_INET;                   // Address Family, Internet
    sin.sin_addr.s_addr = inet_addr(SERVER_IP); // Ip Address in network format
    sin.sin_port = htons(SERVER_PORT);          // Port in network format

    /* Create a socket */
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    int option = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    if (sockfd < 0) // Check for error
    {
        handle_error("Socket Failed", sockfd);
    }

    /* Bind the socket with particular port and address */
    if (bind(sockfd, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
        handle_error("Bind Failed", sockfd);
    }
    printf("Bind Done\n");

    /* Mark socket as passive listening using listen() */
    int error_check = listen(sockfd, MAX_PENDING);
    if (error_check < 0) // Check for error
    {
        handle_error("Listen Failed", sockfd);
    }

    /* Accept the incoming requests */
    while (1)
    {
        new_sockfd = accept(sockfd, (struct sockaddr *)&sin, &len);
        if (new_sockfd < 0) // Error Check
        {
            handle_error("Accept Failed", sockfd);
        }
        printf("Acception Done\n");

        /* Send Station list from server */
        char *station_list = "Select one of the stations\n1. English\n2. Hindi\n";
        if (send(new_sockfd, station_list, MAX_LINE, 0) < 0)
        {
            handle_error("Send Failed", new_sockfd);
        }

        if (recv(new_sockfd, buf, MAX_LINE, 0) < 0)
        {
            handle_error("Station Recieving Failed!", new_sockfd);
        }
        printf("%s\n", buf);

        FILE *fd = fopen("metadata.txt", "rb"); // Open the file in read only mode
    }
    return 0;
}