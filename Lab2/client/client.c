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
#define IP_ADDRESS "127.0.0.1"

void error_handler()
{
    perror("error");
    exit(1);
}

int main(int argc, char *argv[])
{
    /* Defining required variables */
    struct hostent *hp;
    struct sockaddr_in sin;
    struct sockaddr_storage servaddr;
    char *fileName = "sample.mp4";
    socklen_t servaddr_len = servaddr.ss_len;
    char buf[MAX_LINE];
    int s, len, c, r;
    char *host;
    FILE *fp;
    int bytes = 0;
    char new_buf[MAX_LINE];

    if (argc == 2)
    {
        host = argv[1];
    }
    else
    {
        error_handler();
    }
    /* translate host name into peer’s IP address */
    /* gethostbyname() returns a pointer to a hostent struct or NULL.*/
    hp = gethostbyname(host);
    if (!hp)
    {
        error_handler();
    }

    /*Initialize sockaddr struc to memory byte 0*/
    bzero((char *)&sin, sizeof(sin));
    /*Prepare the sockaddr_in structure*/
    sin.sin_family = AF_INET;
    sin.sin_port = htons(SERVER_PORT);
    /*Copy the address of host in hostent to sockaddr*/
    bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);

    /*Creating a socket*/
    s = socket(PF_INET, SOCK_DGRAM, 0);
    if (s < 0)
    {
        error_handler();
    }
    puts("Socket created");

    while (fgets(new_buf, MAX_LINE, stdin))
    {
        new_buf[MAX_LINE - 1] = '\0';
        len = strlen(new_buf) + 1;
        sendto(s, new_buf, MAX_LINE, 0, (const struct sockaddr *) &sin, (socklen_t)sizeof(sin));
        while (bytes = recvfrom(s, buf, MAX_LINE, 0, &servaddr, servaddr_len))
        {
            printf("%s\n", (buf));
            fp = fopen(fileName, "ab");
            if (NULL == fp)
            {
                error_handler();
            }

            fprintf(fp, "%s", buf);

            fclose(fp);
        }
    }
    return 0;
}