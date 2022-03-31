#include "../header.h"

void error_handle(char *msg_to_display, int sock_fd_to_close);

int main(int argc, char *argv[])
{
    /* Declare required variables */
    struct sockaddr_in clientaddr;
    struct hostent *hp;
    int sock_fd, bytes_sent, total_bytes_sent = 0, bytes_recieved, total_bytes_recieved = 0;
    char buf[MAX_LINE];

    /* Initialize variables */
    hp = gethostbyname(SERVER_IP);
    bcopy(hp->h_addr, (char *)&clientaddr.sin_addr, hp->h_length);
    clientaddr.sin_port = htons(SERVER_PORT);
    clientaddr.sin_family = AF_INET;

    /* Create a socket */
    sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        error_handle("Socket Creation Failed!", sock_fd);
    }

    /* Send connection request to server */
    if (connect(sock_fd, (const struct sockaddr *)&clientaddr, sizeof(struct sockaddr_in)) < 0)
    {
        error_handle("Connection refused!", sock_fd);
    }

    /* Send "Hello" to server */
    strcpy(buf, "Hello\0");
    if ((bytes_sent = send(sock_fd, buf, MAX_LINE, 0)) < 0)
    {
        error_handle("Sending \"Hello\" Failed!", sock_fd);
    }

    /* Recieve the question to process */
    bzero(buf, MAX_LINE);
    if ((bytes_recieved = recv(sock_fd, buf, MAX_LINE, 0)) < 0)
    {
        error_handle("Recieving Question Failed!", sock_fd);
    }

    /* Process the sent question */
    int operation_result;
    if (strstr(buf, "+") != NULL)
    {
        char *tokens;
        tokens = strtok(buf, "+");
        int args[2], count = 0;
        while(tokens != NULL)
        {
            printf("%s\n", tokens);
            args[count++] = atoi(tokens);
            tokens = strtok(NULL, "+");
        }
        operation_result = args[0] + args[1];
    }

    return 0;
}

void error_handle(char *msg_to_display, int sock_fd_to_close)
{
    close(sock_fd_to_close);
    perror(msg_to_display);
    exit(EXIT_FAILURE);
}