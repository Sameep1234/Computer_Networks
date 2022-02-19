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
#define MAX_PENDING 5
#define SERVER_IP "127.0.0.1"

void handle_error(char *error_msg, int socket_fd)
{
    close(socket_fd);
    perror(error_msg);
    exit(EXIT_FAILURE);
}

int main()
{
    struct sockaddr_in sin;
    char buf[BUFSIZ];                 // Buffer
    int sockfd, new_sockfd;           // Sockets
    int len;                          // For storing the length of the msg
    bzero((char *)&sin, sizeof(sin)); // Initialize structure

    /* Modify required variables */
    sin.sin_family = AF_INET;                   // Address Family, Internet
    sin.sin_addr.s_addr = inet_addr(SERVER_IP); // Ip Address in network format
    sin.sin_port = htons(SERVER_PORT);          // Port in network format

    /* Create a socket */
    sockfd = socket(PF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) // Check for error
    {
        handle_error("Socket Failed", sockfd);
    }

    /* Bind the socket with particular port and address */
    if (bind(sockfd, (struct sockaddr_in *)&sin, sizeof(sin)) < 0)
    {
        handle_error("Bind Failed", sockfd);
    }

    /* Mark socket as passive listening using listen() */
    int error_check = listen(sockfd, MAX_PENDING);
    if (error_check < 0) // Check for error
    {
        handle_error("Listen Failed", sockfd);
    }

    /* Accept the incoming requests */
    while (1)
    {
        new_sockfd = accept(sockfd, (struct sockaddr_in *)&sin, &len);
        if (new_sockfd < 0) // Error Check
        {
            handle_error("Accept Failed", sockfd);
        }

        /* Send "Hello" from server */
        char *initial_greetings = "Hello";
        if (send(new_sockfd, initial_greetings, sizeof(buf), 0) < 0)
        {
            handle_error("Send Failed", new_sockfd);
        }

        /* Recieve request from client */
        while (len = recv(new_sockfd, buf, sizeof(buf), 0))
        {
            if (buf[0] == 'B' && buf[1] == 'y', buf[2] == 'e') // Terminate the socket if client says bye.
            {
                printf("Bye: Closing Connection\n");
                close(new_sockfd);
                break;
            }
            else
            {
                char *fileName = buf;

                /* Define structure for stat system call */
                struct stat sfile;
                int len_of_file = strlen(fileName); // Get the len of file name
                fileName[len_of_file - 1] = '\0'; // Make the last char to indicate the end of the string

                int status = stat(fileName, &sfile); // Invoke stat system call

                if (status < 0)
                {
                    char *file_not_found = "File not found";
                    if (send(new_sockfd, file_not_found, sizeof(buf), 0) < 0) // Send error over the socket
                    {
                        handle_error("Sending File Failed!", new_sockfd);
                    }
                }

                long double file_size = sfile.st_size; // Get the bytes of required file.

                int fd = open(fileName, O_RDONLY); // Open the file in read only mode
                char *ok = "OK";
                if (send(new_sockfd, ok, sizeof(ok), 0) < 0) // Send "ok" over the socket
                {
                    handle_error("Sending Ok Failed!", new_sockfd);
                }

                while (file_size > 0) // Continue the loop until all the bytes of the file is read.
                {
                    if(file_size > BUFSIZ)
                    {
                        int size_read = read(fd, buf, BUFSIZ); // Read from the file
                        if (size_read < 0)
                        {
                            handle_error("Read SYSCALL Failed!", new_sockfd);
                        }

                        if (send(new_sockfd, buf, BUFSIZ, 0) < 0) // Send the read data over the socket
                        {
                            handle_error("File Send Failed!", new_sockfd);
                        }

                        file_size -= BUFSIZ; // Decrement the file size
                    }
                    else
                    {
                        printf("In here!\n");
                        int size_read = read(fd, buf, file_size);
                        if(size_read < 0)
                        {
                            handle_error("Read SYSCALL Failed in Else!", new_sockfd);
                        }

                        if(send(new_sockfd, buf, file_size, 0) < 0)
                        {
                            handle_error("File Send Failed in else!", new_sockfd);
                        }

                        file_size -= BUFSIZ;
                    }
                }

                close(fd);
            }
        }
    }
    return 0;
}