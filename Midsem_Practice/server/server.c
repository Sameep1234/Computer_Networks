#include "../header.h"

void error_handle(char *msg_to_display, int sock_fd_to_close);

int main(int argc, char *argv[])
{
    /* Declare required variables */
    int sock_fd, new_sock_fd, bytes_recieved, total_bytes_received = 0, bytes_sent, total_bytes_sent = 0;
    char *buf[MAX_LINE];
    struct sockaddr_in servaddr;

    /* Initialize the required variables */
    bzero(&servaddr, sizeof(struct sockaddr_in));
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);

    /* Socket Creation */
    sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        error_handle("Socket Creation Failed", sock_fd);
    }
    printf("Socket Created Succesfully!\n");

    /* Bind socket to particular address and port */
    if (bind(sock_fd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in)) < 0)
    {
        error_handle("Bind Failed!", sock_fd);
    }
    printf("Bind Done!\n");

    /* Passively Listen */
    if (listen(sock_fd, MAX_PENDING) < 0)
    {
        error_handle("Listen Failed!", sock_fd);
    }

    /* Continuously accept incoming requests */
    while (1)
    {
        new_sock_fd = accept(sock_fd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in));
        if (new_sock_fd < 0)
        {
            error_handle("Accept Failed!", sock_fd);
        }

        /* Recieve "Hello" from client */
        while (bytes_recieved = recv(new_sock_fd, buf, MAX_LINE, 0))
        {
            total_bytes_received += bytes_recieved;
            printf("Bytes Recieved are: %d\n", bytes_recieved);
            printf("Recieved response is: %s", buf);

            /* Send question only if "Hello" is recieved */
            if (strcmp(buf, "Hello") == 0)
            {
                /* Save the question in a buffer */
                bzero(buf, MAX_LINE); // Clear the buffer
                strcpy(buf, "What is 13+5?\0");

                /* Send the buffer over socket */
                if ((bytes_sent = send(new_sock_fd, buf, MAX_LINE, 0)) < 0)
                {
                    error_handle("Send Failed!", new_sock_fd);
                }
            }
        }
    }

    return 0;
}