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
    char *fileName = "sample.txt";
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
            .filename = "sample.txt",
            .filename_size = strlen(fileName)};

    struct ACK ack = {.type = 1};

    struct File_info_and_data *fid = malloc(sizeof(struct File_info_and_data));

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
    if (sendto(s, (struct File_request *)&fr, (sizeof(fr)), 0, (const struct sockaddr *)&sin, (socklen_t)sizeof(struct sockaddr_in)) < 0)
    {
        error_handler("ending Failed!");
    }
    printf("File Request Sent. Waiting for Recieving file\n");
    fp = fopen(fr.filename, "wb");
    int prev_seq_no = 1;

    while (1)
    {
        bytes = recvfrom(s, fid, sizeof(struct File_info_and_data), 0, (struct sockaddr *)&servaddr, &servaddr_len);
        if (bytes < 0)
        {
            error_handler("Receving file failed!");
        }

        if (fid->type == 4)
        {
            printf("File Not Found\n");
            break;
        }

        if (prev_seq_no != fid->sequence_number)
        {
            ack.type = 1;
            ack.num_sequences = 1;
            ack.sequence_no[0] = fid->sequence_number;
            if (sendto(s, (struct ACK *)&ack, sizeof(struct ACK), 0, (const struct sockaddr *)&sin, (socklen_t)sizeof(struct sockaddr_in)) < 0)
            {
                error_handler("Sending ACK Failed!");
            }

            printf("Ack sent for sequence number %d. Writing to file now!\n", ack.sequence_no[0]);
            // printf("Values is: %s\n\n\n", fid->data);
            if (strcmp(fid->data, "BYE") == 0)
            {
                printf("EOF Recieved!\n");
                fclose(fp);
                break;
            }
            fwrite(fid->data, 1, strlen(fid->data), fp);
            prev_seq_no = fid->sequence_number;
        }
        else
        {
            ack.type = 1;
            ack.num_sequences = 1;
            ack.sequence_no[0] = fid->sequence_number;
            if (sendto(s, (struct ACK *)&ack, sizeof(struct ACK), 0, (const struct sockaddr *)&sin, (socklen_t)sizeof(struct sockaddr_in)) < 0)
            {
                error_handler("Sending ACK Failed!");
            }

            printf("Ack again sent for sequence number %d. Not writing to file.\n", fid->sequence_number);
        }
    }
    return 0;
}