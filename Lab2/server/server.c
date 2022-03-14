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
    socklen_t clientaddr_len = clientaddr.ss_len;
    struct sockaddr_in sin;
    char buf[MAX_LINE];
    int s, new_s, b, len;
    FILE *fp;
    bzero((char *)&sin, sizeof(sin));

    /*Prepare the sockaddr_in structure*/
    sin.sin_family = AF_INET; //AF_INET is an address family that is used to designate the type of addresses that your socket can communicate with
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

    while (1)
    {
        while (len = recvfrom(s, buf, MAX_LINE, 0, (const struct sockaddr_storage *) &clientaddr, clientaddr_len))
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
            while (fgets(buf, MAX_LINE, fp) != NULL)
            {
                /* First read file in chunks of MAX_LINE bytes */
                printf("%s\n", buf);
                if (sendto(s, buf, MAX_LINE, 0, (const struct sockaddr_storage *) &clientaddr, clientaddr_len) < 0)
                {
                    error_handler();
                }
                bzero(buf, MAX_LINE);
            }

            fclose(fp);

            // strcpy(buf, "!");
            // if (send(new_s, buf, MAX_LINE, 0) < 0)
            // {
            //     error_handler();
            // }
        }
    }
    return 0;
}