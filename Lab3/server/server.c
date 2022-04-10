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
    int s, new_s, b, len, seq_num = 0;
    FILE *fp;
    bzero((char *)&sin, sizeof(sin));

    struct File_request *fr = malloc(sizeof(struct File_request));
    struct ACK *ack = malloc(sizeof(struct ACK));
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
        int bytes_read = 0, total_bytes, loop_count = 0, temp = 0;

        while ((len = recvfrom(s, fr, sizeof(*fr), 0, (struct sockaddr *)&clientaddr, (socklen_t *)&clientaddr_len)) > 0)
        {
            printf("Filename: %s\n", fr->filename);

            fp = fopen(fr->filename, "rb");
            if (fp == NULL)
            {
                // Send File_Not_Found structure
                fnf.type = 4;
                strcpy(fnf.filename, fr->filename);
                fnf.filename_size = fr->filename_size;

                if (sendto(s, (struct File_not_found *)&fnf, sizeof(struct File_not_found), 0, (const struct sockaddr *)&clientaddr, clientaddr_len) < 0)
                {
                    error_handler("Sending FNF Failed!");
                }

                continue;
            }
            else
            {
                while (!feof(fp))
                {
                    bzero(&data, sizeof(struct Data));
                    if (temp == 0)
                    {
                        temp = 1;

                        // Calculate size
                        fseek(fp, SEEK_END, 0);
                        int file_size = ftell(fp);
                        rewind(fp);

                        // Read data
                        bytes_read = fread(buf, 1, MAX_LINE - 1, fp);
                        buf[MAX_LINE - 1] = '\0';

                        // Set Structure variables
                        fid.type = 2;
                        fid.sequence_number = seq_num;
                        fid.filename_size = fr->filename_size;
                        strcpy(fid.filename, fr->filename);
                        fid.file_size = file_size;
                        fid.block_size = BUFSIZ;
                        strcpy(fid.data, buf);

                        // Toggle seq_num
                        seq_num ^= 1;

                        // Send this structure
                        if (sendto(s, (struct File_info_and_data *)&fid, sizeof(struct File_info_and_data), 0, (const struct sockaddr *)&clientaddr, clientaddr_len) < 0)
                        {
                            error_handler("Sending FID Failed!");
                        }

                        // Wait for ack
                        while (1)
                        {
                            len = recvfrom(s, ack, sizeof(struct ACK), 0, (struct sockaddr *)&clientaddr, (socklen_t *)&clientaddr_len);

                            if (len > 0 && ack->sequence_no[0] == fid.sequence_number)
                            {
                                printf("ACK for squence number %d recieved\n", fid.sequence_number);
                                bzero(buf, MAX_LINE);
                                break;
                            }
                            else if (errno == EAGAIN)
                            {
                                printf("ACK for sequence number %d Not Recieved. Sending Frame again.\n", fid.sequence_number);

                                if (sendto(s, (struct Data *)&data, sizeof(struct Data), 0, (const struct sockaddr *)&clientaddr, clientaddr_len) < 0)
                                {
                                    error_handler("Failed to send after ACK not recieved!");
                                }
                            }
                        }
                    }
                    else
                    {
                        loop_count++;
                        bytes_read = fread(buf, 1, MAX_LINE - 1, fp);
                        buf[MAX_LINE - 1] = '\0';

                        if (ferror(fp) != 0)
                        {
                            error_handler("Failed to read file!");
                        }
                        // printf("Bytes Send: %d\n", bytes_read);

                        total_bytes += bytes_read;

                        data.block_size = BUFSIZ;
                        data.sequence_number = seq_num;
                        strcpy(data.data, buf);

                        seq_num ^= 1;

                        if (sendto(s, (struct Data *)&data, sizeof(struct Data), 0, (const struct sockaddr *)&clientaddr, clientaddr_len) < 0)
                        {
                            error_handler("Failed to send through socket!");
                        }
                        bzero(buf, MAX_LINE);
                        while (1)
                        {
                            len = recvfrom(s, ack, sizeof(struct ACK), 0, (struct sockaddr *)&clientaddr, (socklen_t *)&clientaddr_len);

                            if (len > 0 && ack->sequence_no[0] == data.sequence_number)
                            {
                                printf("ACK for squence number %d recieved\n", data.sequence_number);
                                bzero(buf, MAX_LINE);
                                break;
                            }
                            else if (errno == EAGAIN)
                            {
                                printf("ACK for sequence number %d Not Recieved. Sending Frame again.\n", fid.sequence_number);

                                if (sendto(s, (struct Data *)&data, sizeof(struct Data), 0, (const struct sockaddr *)&clientaddr, clientaddr_len) < 0)
                                {
                                    error_handler("Failed to send after ACK not recieved!");
                                }
                            }
                        }
                    }
                }
                printf("Sending Bye!\n");
                bzero(&data, sizeof(struct Data));
                data.block_size = BUFSIZ;
                printf("Sequence number final is: %d\n", seq_num);
                data.sequence_number = seq_num;
                strcpy(data.data, "BYE\0");
                printf("%s\n", data.data);
                if (sendto(s, (struct Data *)&data, sizeof(struct Data), 0, (const struct sockaddr *)&clientaddr, clientaddr_len) < 0)
                {
                    error_handler("Failed to send \" BYE \"!");
                }

                while (1)
                {
                    len = recvfrom(s, ack, sizeof(struct ACK), 0, (struct sockaddr *)&clientaddr, (socklen_t *)&clientaddr_len);

                    if (len > 0 && ack->sequence_no[0] == data.sequence_number)
                    {
                        printf("ACK for squence number %d recieved\n", data.sequence_number);
                        bzero(buf, MAX_LINE);
                        break;
                    }
                    else if (errno == EAGAIN)
                    {
                        printf("ACK for sequence number %d Not Recieved. Sending Frame again.\n", data.sequence_number);

                        if (sendto(s, (struct Data *)&data, sizeof(struct Data), 0, (const struct sockaddr *)&clientaddr, clientaddr_len) < 0)
                        {
                            error_handler("Failed to send after ACK not recieved!");
                        }
                    }
                }
            }
        }
    }
    return 0;
}