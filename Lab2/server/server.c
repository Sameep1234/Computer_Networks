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
#include <time.h>
#define SERVER_PORT 8080
#define MAX_LINE BUFSIZ

/* Error Function */
void error_handler(char *error_msg)
{
    perror(error_msg);
    exit(EXIT_FAILURE);
}

int main()
{
    /* Defining required variables */
    struct sockaddr_storage clientaddr;
    char *fileName = "sample.mp4";
    socklen_t clientaddr_len = sizeof(clientaddr);
    struct sockaddr_in sin;
    char buf[MAX_LINE];
    int s, new_s, b, len;
    FILE *fp;

    /* Clearing the memory */
    bzero((char *)&sin, sizeof(sin));

    /*Prepare the sockaddr_in structure*/
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr("127.0.0.1");
    sin.sin_port = htons(SERVER_PORT);

    /* Creating socket */
    s = socket(PF_INET, SOCK_DGRAM, 0);
    if (s < 0)
    {
        error_handler("socket creation failure");
    }
    puts("Socket created");

    /* Binding the socket */
    b = bind(s, (struct sockaddr *)&sin, sizeof(sin));
    if (b < 0)
    {
        error_handler("bind error");
    }
    puts("Bind done");

    while (1)
    {
        int bytes_read = 0, total_bytes, loop_count = 0;
        /* Receive "GET" from client */
        while ((len = recvfrom(s, buf, MAX_LINE, 0, (struct sockaddr *)&clientaddr, (socklen_t *)&clientaddr_len)) > 0)
        {
            if (strcmp(buf, "GET") == 0)
            {
                printf("%s\n", buf);
                /* Clearing the memory */
                bzero(buf, sizeof(buf));
                /* Opening the file in rb mode. */
                fp = fopen(fileName, "rb");
                if (fp == NULL)
                {
                    error_handler("Opening file failed!");
                }

                /* Continue the loop until all the bytes of the file is read. */
                while (!feof(fp))
                {
                    /* In our case the optimal delay is 0.001s */
                    /* 
                        But to show while transferring we are streaming, 
                        we chose delay = 0.01s
                    */
                    struct timespec t;
                    t.tv_nsec = 10000000L;
                    nanosleep((const struct timespec *)&t, NULL);

                    loop_count++;
                    bytes_read = fread(buf, 1, MAX_LINE - 1, fp);
                    buf[MAX_LINE - 1] = '\0';
                    if (ferror(fp) != 0)
                    {
                        error_handler("Error in reading file");
                    }
                    printf("Bytes Send: %d\n", bytes_read);
                    total_bytes += bytes_read;

                    /* Send the read data over the socket to client. */
                    if (sendto(s, buf, bytes_read, 0, (const struct sockaddr *)&clientaddr, clientaddr_len) < 0)
                    {
                        error_handler("send error");
                    }
                    /* Clearing the memory */
                    bzero(buf, MAX_LINE);
                }
            }

            printf("Total Bytes sent: %d\n", total_bytes);
            printf("Loop Count: %d\n", loop_count);
            /* Close the file. */  
            fclose(fp);

            /* Sending "BYE" to indicate EOF. */
            printf("Sending Bye!\n");
            strcpy(buf, "BYE\0");
            if (sendto(s, buf, MAX_LINE - 1, 0, (const struct sockaddr *)&clientaddr, clientaddr_len) < 0)
            {
                error_handler("error sending bye!");
            }
        }
    }
    return 0;
}