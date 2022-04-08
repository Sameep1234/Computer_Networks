#include "../header.h"

void error_handler(char *error_msg)
{
    perror(error_msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    /* Defining required variables */
    struct hostent *hp;
    struct sockaddr_in sin;
    struct sockaddr_storage servaddr;
    char *fileName = "sample.mp4";
    socklen_t servaddr_len = sizeof(struct sockaddr);
    char buf[MAX_LINE];
    int s, len, c, r;
    char *host;
    FILE *fp;
    int bytes = 0;
    char new_buf[MAX_LINE];

    /* Required Structs */
    struct File_request fr =
        {
            .type = 0,
            .filename = fileName,
            .filename_size = strlen(fileName)};

    struct ACK ack = {.type = 1};

    struct File_info_and_data fid;
    struct Data data;
    struct File_not_found fnf;

    if (argc == 2)
    {
        host = argv[1];
    }
    else
    {
        error_handler("Argument Count Invalid!");
    }
    /* translate host name into peer’s IP address */
    /* gethostbyname() returns a pointer to a hostent struct or NULL.*/
    hp = gethostbyname(host);
    if (!hp)
    {
        error_handler("Host entry Failed!");
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
        error_handler("Socket creation failure");
    }
    puts("Socket created");
    if(sendto(s, (const char *) &fr, MAX_LINE - 1, 0, (const struct sockaddr *)&sin, (socklen_t)sizeof(struct sockaddr_in)) < 0)
    {
        error_handler("ending Failed!");
    }
    printf("Send\n");
    int temp = 0;
    while (1)
    {
        bytes = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&servaddr, &servaddr_len);

        printf("Block 1 : %s\n", buf);
        bzero(buf, MAX_LINE);

        printf("Before Sleep\n");
        /* if (temp == 0)
        {
            struct timespec t;
            t.tv_sec = 3;
            // t.tv_nsec = 500000000L;
            nanosleep((const struct timespec *)&t, NULL);
            temp = 1;
        } */

        strcpy(buf, "ACK\0");
        printf("Sending ACK after sleep\n");
        sendto(s, buf, MAX_LINE - 1, 0, (const struct sockaddr *)&sin, (socklen_t)sizeof(struct sockaddr_in));
    }

    return 0;
}