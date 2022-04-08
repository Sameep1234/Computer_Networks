#include "../header.h"

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
    bzero((char *)&sin, sizeof(sin));

    struct File_request fr;
    struct ACK ack;
    struct File_info_and_data fid;
    struct Data data;
    struct File_not_found fnf;

    /*Prepare the sockaddr_in structure*/
    sin.sin_family = AF_INET; // AF_INET is an address family that is used to designate the type of addresses that your socket can communicate with
    sin.sin_addr.s_addr = inet_addr("127.0.0.1");
    sin.sin_port = htons(SERVER_PORT);

    /* Creating socket */
    s = socket(PF_INET, SOCK_DGRAM, 0);
    if (s < 0)
    {
        error_handler("socket creation failure");
    }
    puts("Socket created");

    struct timeval tv;
    tv.tv_sec = 1;
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval)) < 0)
    {
        error_handler("Timeout Setting Failed!");
    }

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

        while ((len = recvfrom(s, buf, MAX_LINE, 0, (struct sockaddr *)&clientaddr, (socklen_t *)&clientaddr_len)) > 0)
        {
            bcopy(buf, &fr, strlen(buf));
            printf("Filename: %s\n", fr.filename);
            
            /* if (len > 0 && strcmp(buf, "GET") == 0)
            {
                printf("%s\n", buf);
                bzero(buf, MAX_LINE);
                sendto(s, buf, MAX_LINE - 1, 0, (const struct sockaddr *)&clientaddr, clientaddr_len);
                bzero(buf, MAX_LINE);
            }
            while (1)
            {
                len = recvfrom(s, buf, MAX_LINE, 0, (struct sockaddr *)&clientaddr, (socklen_t *)&clientaddr_len);

                if (len > 0 && strcmp(buf, "ACK") == 0)
                {
                    printf("%s\n", buf);
                    bzero(buf, MAX_LINE);
                    break;
                }
                else if (errno == EAGAIN)
                {
                    printf("ACK Not Recieved\n");
                    bzero(buf, MAX_LINE);
                    strcpy(buf, "Hi\0");
                    sendto(s, buf, MAX_LINE - 1, 0, (const struct sockaddr *)&clientaddr, clientaddr_len);
                }
            } */
        }
    }
    return 0;
}