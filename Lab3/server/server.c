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

/*
    THINGS TO CHANGE
    FIleName
    filename length
    sequence number
*/

struct File_request
{
    uint8_t type;
    uint8_t filename_size;
    char *filename;
};

struct ACK
{
    uint8_t type;
    uint8_t num_sequences;
    uint16_t *sequence_no;
};

struct File_info_and_data
{
    uint8_t type;
    uint16_t sequence_number;
    uint8_t filename_size;
    char *filename;
    uint32_t file_size;
    uint16_t block_size;
    char *data;
};

struct Data
{
    uint8_t type;
    uint16_t sequence_number;
    uint16_t block_size;
    char *data;
};

struct File_not_found
{
    uint8_t type;
    uint8_t filename_size;
    char *filename;
};

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
    char buf[MAX_LINE], new_buf[MAX_LINE];
    int s, new_s, b, len;
    FILE *fp;

    /* Initialize variables */
    struct File_info_and_data fid =
        {
            .type = 2,
            .sequence_number = 5,
            .filename_size = 10,
            .filename = "sample.mp4",
            .file_size = 10,
            .block_size = MAX_LINE,
            .data = buf};

    struct Data data =
        {
            .type = 3,
            .sequence_number = 5,
            .block_size = MAX_LINE,
            .data = buf};

    struct File_not_found fnf =
        {
            .type = 4,
            .filename_size = 10,
            .filename = "sample.mp4"};

    struct File_request *fr;
    uint16_t temp_array = {1, 2, 3, 4, 5};
    struct ACK *_ack;

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
        /* Receive File Request from client */
        while ((len = recvfrom(s, buf, MAX_LINE, 0, (struct sockaddr *)&clientaddr, (socklen_t *)&clientaddr_len)) > 0)
        {
            fr = (struct File_request *)buf;
            fid.filename = fr->filename;
            fid.filename_size = fr->filename_size;

            /* Calculate File Size */
            FILE *fp = fopen(fr->filename, "rb");
            fseek(fp, SEEK_END, 0);
            fid.file_size = ftell(fp);
            printf("File Size is: %d\n", fid.file_size);
            rewind(fp);

            /* Read only first time the first block of data from file. */
            int bytes_read = fread(fid.data, 1, MAX_LINE - 1, fp);
            if (bytes_read < 0)
            {
                error_handler("First File Read Failed!");
            }
            int bytes_send = 0;

            if ((bytes_send = sendto(s, (const char *)&fid, sizeof(struct File_info_and_data), 0, (const struct sockaddr *)&clientaddr, clientaddr_len)) < 0)
            {
                error_handler("FID Struct Failed!");
            }

            /* Set Sock Option for async timeout */
            struct timespec t;
            t.tv_nsec = 10000000L;
            setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(struct timespec));
            int bytes_recieved = 0;
            bzero(buf, MAX_LINE);
            bzero(new_buf, MAX_LINE);
            if((bytes_recieved = recvfrom(s, buf, MAX_LINE, 0, (struct sockaddr *)&clientaddr, (socklen_t *)&clientaddr_len)) < 0)
            {
            }
        }
    }
    return 0;
}

// if (strcmp(buf, "GET") == 0)
//             {
//                 printf("%s\n", buf);
//                 /* Clearing the memory */
//                 bzero(buf, sizeof(buf));
//                 /* Opening the file in rb mode. */
//                 fp = fopen(fileName, "rb");
//                 if (fp == NULL)
//                 {
//                     error_handler("Opening file failed!");
//                 }

//                 /* Continue the loop until all the bytes of the file is read. */
//                 while (!feof(fp))
//                 {
//                     /* In our case the optimal delay is 0.001s */
//                     /*
//                         But to show while transferring we are streaming,
//                         we chose delay = 0.01s
//                     */
//                     struct timespec t;
//                     t.tv_nsec = 10000000L;
//                     nanosleep((const struct timespec *)&t, NULL);

//                     loop_count++;
//                     bytes_read = fread(buf, 1, MAX_LINE - 1, fp);
//                     buf[MAX_LINE - 1] = '\0';
//                     if (ferror(fp) != 0)
//                     {
//                         error_handler("Error in reading file");
//                     }
//                     printf("Bytes Send: %d\n", bytes_read);
//                     total_bytes += bytes_read;

//                     /* Send the read data over the socket to client. */
//                     if (sendto(s, buf, bytes_read, 0, (const struct sockaddr *)&clientaddr, clientaddr_len) < 0)
//                     {
//                         error_handler("send error");
//                     }
//                     /* Clearing the memory */
//                     bzero(buf, MAX_LINE);
//                 }
//             }

//             printf("Total Bytes sent: %d\n", total_bytes);
//             printf("Loop Count: %d\n", loop_count);
//             /* Close the file. */
//             fclose(fp);

//             /* Sending "BYE" to indicate EOF. */
//             printf("Sending Bye!\n");
//             strcpy(buf, "BYE\0");
//             if (sendto(s, buf, MAX_LINE - 1, 0, (const struct sockaddr *)&clientaddr, clientaddr_len) < 0)
//             {
//                 error_handler("error sending bye!");
//             }