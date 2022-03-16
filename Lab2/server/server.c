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

void error_handler()
{
    perror("error");
    exit(1);
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
    bzero((char *)&sin, sizeof(sin));

    /*Prepare the sockaddr_in structure*/
    sin.sin_family = AF_INET; // AF_INET is an address family that is used to designate the type of addresses that your socket can communicate with
    sin.sin_addr.s_addr = inet_addr("127.0.0.1");
    sin.sin_port = htons(SERVER_PORT);

    /* Creating socket */
    s = socket(PF_INET, SOCK_DGRAM, 0);
    if (s < 0)
    {
        error_handler();
    }
    puts("Socket created");

    /* Binding the socket */
    b = bind(s, (struct sockaddr *)&sin, sizeof(sin));
    if (b < 0)
    {
        error_handler();
    }
    puts("Bind done");
    int bytes_read = 0, total_bytes, loop_count = 0;

    while (1)
    {

        while ((len = recvfrom(s, buf, MAX_LINE, 0, (struct sockaddr *)&clientaddr, (socklen_t *)&clientaddr_len)))
        {
            if (strcmp(buf, "GET") == 0)
            {
                printf("%s\n", buf); // "GET" coming or not!
                /* Open the file that we wish to transfer */

                fp = fopen(fileName, "rb");
                if (fp == NULL)
                {
                    error_handler();
                }

                /* Read data from file and send it */
                bzero(buf, sizeof(buf));

                while (!feof(fp)) // Continue the loop until all the bytes of the file is read.
                {
                    struct timespec t;
                    printf("Here1\n");
                    t.tv_sec = 1;
                    printf("Here2\n");
                    nanosleep((const struct timespec *)&t, NULL);
                    loop_count++;
                    bytes_read = fread(buf, 1, MAX_LINE - 1, fp);
                    buf[MAX_LINE - 1] = '\0';
                    // printf("%s\n", buf);
                    if (ferror(fp) != 0)
                    {
                        error_handler();
                    }
                    printf("Bytes Send: %d\n", bytes_read);
                    // printf("BUF CONTENTS: %s\n", buf);
                    total_bytes += bytes_read;
                    if (sendto(s, buf, sizeof(buf), 0, (const struct sockaddr *)&clientaddr, clientaddr_len) < 0) // Send the read data over the socket
                    {
                        error_handler();
                    }
                    bzero(buf, MAX_LINE); // Erase the previous data
                
            }
            printf("Total Bytes sent: %d\n", total_bytes);
            printf("Loop Count: %d\n", loop_count);

            fclose(fp);
            }
            else
            {
                error_handler();
            }
            printf("Sending Bye!\n");
            strcpy(buf, "BYE\0");
            if (sendto(s, buf, MAX_LINE - 1, 0, (const struct sockaddr *)&clientaddr, clientaddr_len) < 0)
            {
                error_handler();
            }
        }
    }
    return 0;
}